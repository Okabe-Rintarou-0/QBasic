#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>
#include <future>
#include "stringutils.h"
#include "statement.h"
#include "lexer.h"
#include "parser.h"
#include "table.h"

using Statement = statement::Statement;
using Lexer = lexer::Lexer;
using Parser = parser::Parser;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          ui(new Ui::MainWindow),
          venv(std::make_unique<env::Table<std::string, env::Value>>()),
          tenv(std::make_unique<env::Table<std::string, env::valueType>>()),
          lexer(std::make_unique<Lexer>()), parser(std::make_unique<Parser>()) {
    ui->setupUi(this);

    QApplication::connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::load);
    QApplication::connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::run);
    QApplication::connect(ui->cmdLineEdit, &QLineEdit::textChanged, this, &MainWindow::controlCmdlineInput);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addStatement(Statement *stmt) {
    int tgtLineno = stmt->getLineno();
    auto it = statements.begin();
    for (; it != statements.end(); ++it) {
        int curLineno = (*it)->getLineno();
        if (curLineno > tgtLineno) {
            break;
        }
    }
    statements.insert(it, stmt);
}

void MainWindow::controlCmdlineInput() {
    {
        std::lock_guard <std::mutex> lock(mtx);
        if (runningState != INPUTING) return;
    }
    std::string cmdline = ui->cmdLineEdit->text().trimmed().toStdString();
    if (!StringUtils::startWith(cmdline, "? ")) {
        ui->cmdLineEdit->setText("? ");
    }
}

void MainWindow::error(const std::string &errorMsg) {
    QMessageBox::warning(this, "Warning", QString::fromStdString(errorMsg), QMessageBox::Ok);
}

void MainWindow::error(const char *format ...) {
    va_list ap; /* points to each unnamed arg in turn */
    const char *p, *sval;

    int ival;
    double dval;

    std::string result;

    va_start(ap, format);   /* make ap point to 1st unnamed arg */

    for (p = format; *p; p++) {
        if (*p != '%') {
            result += *p;
            continue;
        }
        switch (*++p) {
            case 'd':
                ival = va_arg(ap, int);
                result += std::to_string(ival);
                break;
            case 'f':
                dval = va_arg(ap, double);
                printf("%f", dval);
                result += std::to_string(dval);
                break;
            case 's':
                for (sval = va_arg(ap, char * ); *sval; sval++)
                    result += *sval;
                break;
            default:
                result += *p;
                break;
        }
    }
    va_end(ap); /* clean up when done */
    error(result);
}

void MainWindow::addRawStatement(RawStatement *rawStmt) {
    int tgtLineno = rawStmt->lineno;
    auto it = rawStatements.begin();
    for (; it != rawStatements.end(); ++it) {
        int curLineno = (*it)->lineno;
        if (curLineno > tgtLineno) {
            break;
        }
    }
    rawStatements.insert(it, rawStmt);
}

void MainWindow::init() {
    for (auto stmt: statements) {
        delete stmt;
    }
    statements.clear();
    ui->treeDisplay->clear();
}

void MainWindow::run() {
    try {
        init();
        for (auto rawStmt: rawStatements) {
            auto tokens = lexer->scan(rawStmt->srcCode);
            for (auto token: tokens) {
                std::cout << "read token: " << token << std::endl;
            }
            auto stmt = parser->parse(rawStmt->lineno, rawStmt->srcCode, tokens);
            addStatement(stmt);
            std::string str;
            stmt->print(str);
            ui->treeDisplay->append(QString::fromStdString(str));
        }
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    } catch (const char *errorMsg) {
        std::cerr << errorMsg << std::endl;
        error(errorMsg);
    }
}

void MainWindow::refreshCode() {
    std::string code;
    for (auto rawStmt: rawStatements) {
        code.append(rawStmt->toString() + "\n");
    }
    ui->codeDisplay->setText(QString::fromStdString(code));
}

void MainWindow::load() {
    static std::string lastLoadedDir = "./";
    std::string fileName = QFileDialog::getOpenFileName(this, "Open text file", QString::fromStdString(lastLoadedDir),
                                                        "Text Files(*.txt);;").toStdString();

    auto npos = fileName.find_last_of("/\\");
    if (npos != std::string::npos) {
        lastLoadedDir = fileName.substr(0, npos);
    }

    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        if (file.eof()) break;
        if (line.empty()) continue;
        try {
            RawStatement *rawStmt = RawStatement::fromCmdline(line);
            addRawStatement(rawStmt);
        }
        catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        } catch (const char *errorMsg) {
            std::cerr << errorMsg << std::endl;
            error(errorMsg);
        }
    }
    refreshCode();
    file.close();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            auto cmdline = ui->cmdLineEdit->text().trimmed().toStdString();
            if (cmdline.size() == 0) return;

            if (runningState == INPUTING) {
                inputValue = StringUtils::getAfter(cmdline, "? ");
                inputCv.notify_all();
                inputWorker->join();
                delete inputWorker;
                inputWorker = nullptr;
                runningState = RUNNING;
                goto clear;
            }

            try {
                if (isBuiltinCmd(cmdline)) {
                    runBuiltinCmd(cmdline);
                    goto clear;
                }
                RawStatement *rawStmt = RawStatement::fromCmdline(cmdline);
                addRawStatement(rawStmt);
                refreshCode();
            }
            catch (std::exception e) {
                std::cerr << e.what() << std::endl;
            } catch (const char *errorMsg) {
                std::cerr << errorMsg << std::endl;
                error(errorMsg);
            }
            clear:
            ui->cmdLineEdit->clear();
            break;
        }
    }
}

void MainWindow::print(const std::string &cmdline) {
    std::string var = StringUtils::getAfter(cmdline, "PRINT ");
    env::valueType *typePtr = tenv->look(var);
    if (typePtr != nullptr) {
        env::valueType type = *typePtr;
        env::Value value = *(venv->look(var));
        switch (type) {
            case env::INT:
                ui->resultBrowser->append(QString::number(value.getInt()));
                break;
            case env::STRING:
                ui->resultBrowser->append(QString::fromStdString(value.getString()));
                break;
            default:
                break;
        }

    }
    return;
}

void MainWindow::inputInBackGround(const std::string &cmdline) {
    static std::regex intFmt("([1-9][0-9]*)|0");
    static std::regex strFmt("\".*\"");
    std::string var = StringUtils::getAfter(cmdline, "INPUT ");
    std::unique_lock <std::mutex> lock(mtx);
    inputCv.wait(lock);

    std::cout << "inputValue: " << inputValue << std::endl;
    if (std::regex_match(inputValue, intFmt)) {
        tenv->enter(var, env::INT);
        std::cout << tenv->look(var) << std::endl;
        venv->enter(var, env::Value(std::atoi(inputValue.c_str())));
    } else if (std::regex_match(inputValue, strFmt)) {
        tenv->enter(var, env::STRING);
        venv->enter(var, env::Value(inputValue.substr(1, inputValue.size() - 2)));
    }
    inputValue.clear();
}

void MainWindow::input(const std::string &cmdline) {
    mtx.lock();
    if (runningState == INPUTING) return;
    runningState = INPUTING;
    mtx.unlock();
    ui->cmdLineEdit->setText("? ");
    inputWorker = new std::thread(&MainWindow::inputInBackGround, this, cmdline);
}

bool MainWindow::isBuiltinCmd(const std::string &cmdline) const {
    static std::regex pattern(
            "LIST|RUN|LOAD|(PRINT [a-zA-Z][a-zA-Z0-9]*)|(INPUT [a-zA-Z][a-zA-Z0-9]*)|CLEAR|HELP|QUIT");
    return std::regex_match(cmdline, pattern);
}

void MainWindow::runBuiltinCmd(const std::string &cmdline) {
    std::cout << "run: " << cmdline << std::endl;
    if (cmdline == "LIST") {
        return;
    }

    if (cmdline == "QUIT") {
        QApplication::quit();
    }

    if (cmdline == "RUN") {
        run();
        return;
    }

    if (cmdline == "LOAD") {
        load();
        return;
    }

    if (StringUtils::startWith(cmdline, "PRINT")) {
        print(cmdline);
        return;
    }

    if (StringUtils::startWith(cmdline, "INPUT")) {
        input(cmdline);
        return;
    }

    if (cmdline == "CLEAR") {
        return;
    }

    if (cmdline == "HELP") {
        return;
    }
}
