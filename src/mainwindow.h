#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <memory>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "table.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace statement {
    class RawStatement;

    class Statement;
}

namespace lexer {
    class Lexer;
}

namespace parser {
    class Parser;
}

namespace syntax {
    class Exp;
}

using RawStatement = statement::RawStatement;
using Statement = statement::Statement;
using Lexer = lexer::Lexer;
using Parser = parser::Parser;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    enum RunningState {
        INPUT,
        RUNNING,
        END,
        INIT,
        NONE,
    } runningState = END, lastRunningState = NONE;

    Ui::MainWindow *ui;

    std::unique_ptr <env::Table<std::string, env::Value>> venv;
    std::unique_ptr <env::Table<std::string, env::ValueType>> tenv;

    std::thread *inputWorker = nullptr;

    std::mutex mtx;

    std::string inputValue;

    std::condition_variable inputCv;

    std::list<RawStatement *>::const_iterator stmtIter;

    std::list<RawStatement *> rawStatements;

    std::list<Statement *> statements;

    std::unique_ptr <Lexer> lexer;

    std::unique_ptr <Parser> parser;

    void addStatement(Statement *stmt);

    void addRawStatement(RawStatement *rawStmt);

    bool isBuiltinCmd(const std::string &cmdline) const;

    void runBuiltinCmd(const std::string &cmdline);

    void refreshCode();

    void controlCmdlineInput();

    void gotoLine(int lineno);

    void load();

    void help();

    void run();

    void init();

    void clear();

    void end();

    void print(const std::string &cmdline);

    void inputInBackGround(const std::string &cmdline);

    void input(const std::string &var);

    void info(const std::string &infoMsg);

    void error(const std::string &errorMsg);

    void error(const char *format, ...);

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
