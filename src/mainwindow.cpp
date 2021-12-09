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
          tenv(std::make_unique<env::Table<std::string, env::ValueType>>()),
          lexer(std::make_unique<Lexer>()), parser(std::make_unique<Parser>()) {
    ui->setupUi(this);

    QApplication::connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::load);
    QApplication::connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::run);
    QApplication::connect(ui->btnClearCode, &QPushButton::clicked, this, &MainWindow::clear);
    QApplication::connect(ui->cmdLineEdit, &QLineEdit::textChanged, this, &MainWindow::controlCmdlineInput);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::controlCmdlineInput() {
    {
        std::lock_guard <std::mutex> lock(mtx);
        if (runningState != INPUT) return;
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
    ui->resultBrowser->clear();

    stmtIdx = 0;
}

void MainWindow::info(const std::string &infoMsg) {
    QMessageBox::information(this, "Information", QString::fromStdString(infoMsg), QMessageBox::Ok);
}

void MainWindow::help() {
    std::string infoMsg;
    infoMsg += "Command List: \n";
    infoMsg += "RUN: run the code.\n";
    infoMsg += "LIST: not supported here, do nothing.\n";
    infoMsg += "CLEAR: clear the code and the result.\n";
    infoMsg += "LOAD: load code from disk.\n";
    infoMsg += "HELP: get help tips.\n";
    infoMsg += "INPUT: input a variable. Format: INPUT [variable_name]. e.g., INPUT x\n";
    infoMsg += "PRINT: print the value of given variable. Format: PRINT [variable_name]. e.g PRINT x\n";
    infoMsg += "QUIT: quit QBasic immediately.";

    info(infoMsg);
}

void MainWindow::parseAndPrint() {
    for (auto rawStmt:rawStatements) {
        try {
            auto tokens = lexer->scan(rawStmt->srcCode);
            for (auto token: tokens) {
                std::cout << "read token: " << token << std::endl;
            }

            // Parse stmt.
            auto stmt = parser->parse(rawStmt->lineno, rawStmt->srcCode, tokens);

            stmt->checkValidation(this);

            // Print the syntax tree of the stmt.
            std::string str;
            stmt->print(str);
            ui->treeDisplay->append(QString::fromStdString(str));

            // Add stmt.
            statements.push_back(stmt);

        } catch (const std::string &errorMsg) {
            std::cerr << errorMsg << std::endl;
        }
        catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        } catch (const char *errorMsg) {
            std::cerr << errorMsg << std::endl;
            error(errorMsg);
            ui->treeDisplay->append(QString::number(rawStmt->lineno).append(" Error\n"));
            // If invalid, add nullptr.
            statements.push_back(nullptr);
        }
    }
}

void MainWindow::run() {
    lastRunningState = runningState;
    runningState = RUNNING;
    if (lastRunningState != INPUT)
        init();

    parseAndPrint();

    int len = statements.size();
    for (; stmtIdx < len;) {
        auto stmt = statements[stmtIdx];

        try {
            ++stmtIdx;

            // stmt == nullptr means it's invalid.
            if (stmt == nullptr) continue;

            // Run the stmt.
            stmt->run(this, ui->resultBrowser);

            // Special judge, if input, then break, until input complete.
            if (typeid(*stmt) == typeid(statement::InputStatement))
                break;
        } catch (const std::string &errorMsg) {
            std::cerr << errorMsg << std::endl;
        }
        catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        } catch (const char *errorMsg) {
            std::cerr << errorMsg << std::endl;
            error(errorMsg);
        }
    }

    if (stmtIdx == len) {
        lastRunningState = RUNNING;
        runningState = END;
    }
}

void MainWindow::refreshCode() {
    std::string code;
    for (auto rawStmt: rawStatements) {
        code.append(rawStmt->toString() + "\n");
    }
    ui->codeDisplay->setText(QString::fromStdString(code));
}

void MainWindow::clear() {
    for (auto rawStmt: rawStatements) {
        delete rawStmt;
    }
    rawStatements.clear();
    for (auto stmt: statements) {
        if (stmt) delete stmt;
    }
    statements.clear();

    ui->codeDisplay->clear();
    ui->treeDisplay->clear();
    ui->resultBrowser->clear();
    ui->cmdLineEdit->clear();

    tenv->clear();
    venv->clear();

    lastRunningState = runningState;
    runningState = END;
}

void MainWindow::gotoLine(int lineno) {
    int len = statements.size();
    for (int i = 0; i < len; ++i) {
        if (statements[i]->getLineno() == lineno) {
            stmtIdx = i;
            return;
        }
    }
    throw "Use non-existent line number!";
}

void MainWindow::deleteLine(int lineno) {
    for (auto tmpIter = rawStatements.begin(); tmpIter != rawStatements.end(); ++tmpIter) {
        if ((*tmpIter)->lineno == lineno) {
            rawStatements.erase(tmpIter);
            refreshCode();
            return;
        }
    }
    throw "Use non-existent line number!";
}

void MainWindow::load() {
    static std::string lastLoadedDir = "./";
    std::string fileName = QFileDialog::getOpenFileName(this, "Open text file", QString::fromStdString(lastLoadedDir),
                                                        "Text Files(*.txt);;").toStdString();

    auto npos = fileName.find_last_of("/\\");
    if (npos != std::string::npos) {
        lastLoadedDir = fileName.substr(0, npos);
    }

    clear();

    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::cout << "read line: " << line << std::endl;
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
        if (file.eof()) break;
    }
    refreshCode();
    file.close();
}

void MainWindow::end() {
    stmtIdx = statements.size();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            auto cmdline = ui->cmdLineEdit->text().trimmed().toStdString();
            if (cmdline.size() == 0) return;

            if (runningState == INPUT) {
                inputValue = StringUtils::getAfter(cmdline, "? ");
                inputCv.notify_all();
                inputWorker->join();
                delete inputWorker;
                inputWorker = nullptr;
                if (lastRunningState == RUNNING) {
                    run();
                } else {
                    runningState = lastRunningState;
                }
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
            catch (const std::string &errorMsg) {
                std::cerr << errorMsg << std::endl;
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
    env::ValueType *typePtr = tenv->look(var);
    if (typePtr != nullptr) {
        env::ValueType type = *typePtr;
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
    } else {
        throw "Use undefined variable!";
    }
    return;
}

void MainWindow::inputInBackGround(const std::string &var) {
    static std::regex intFmt("([1-9][0-9]*)|0");
    static std::regex strFmt("\".*\"");
    std::unique_lock <std::mutex> lock(mtx);
    inputCv.wait(lock);

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

void MainWindow::input(const std::string &var) {
    {
        std::lock_guard <std::mutex> lock(mtx);
        if (runningState == INPUT) return;
        lastRunningState = runningState;
        runningState = INPUT;
    }
    ui->cmdLineEdit->setText("? ");
    inputWorker = new std::thread(&MainWindow::inputInBackGround, this, var);
}

bool MainWindow::isBuiltinCmd(const std::string &cmdline) const {
    static std::regex pattern(
            "([1-9][0-9]*)|LIST|RUN|LOAD|(PRINT [a-zA-Z][a-zA-Z0-9]*)|(INPUT [a-zA-Z][a-zA-Z0-9]*)|CLEAR|HELP|QUIT");
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
        input(StringUtils::getAfter(cmdline, "INPUT "));
        return;
    }

    if (cmdline == "CLEAR") {
        clear();
        return;
    }

    if (cmdline == "HELP") {
        help();
        return;
    }

    deleteLine(std::atoi(cmdline.c_str()));
}
