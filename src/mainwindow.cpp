#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>
#include "statement.h"
#include "lexer.h"
#include "parser.h"

using Statement = statement::Statement;
using Lexer = lexer::Lexer;
using Parser = parser::Parser;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), lexer(std::make_unique<Lexer>()),
          parser(std::make_unique<Parser>()) {
    ui->setupUi(this);

    QApplication::connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::load);
    QApplication::connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::run);
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
            ui->cmdLineEdit->clear();
            try {
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
            break;
        }
    }
}
