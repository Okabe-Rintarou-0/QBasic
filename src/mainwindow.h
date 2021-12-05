#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <memory>
#include <list>

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

using RawStatement = statement::RawStatement;
using Statement = statement::Statement;
using Lexer = lexer::Lexer;
using Parser = parser::Parser;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;

    std::list<RawStatement *> rawStatements;

    std::list<Statement *> statements;

    std::unique_ptr <Lexer> lexer;

    std::unique_ptr <Parser> parser;

    void addStatement(Statement *stmt);

    void addRawStatement(RawStatement *rawStmt);

    void refreshCode();

    void load();

    void run();

    void init();

    void error(const std::string &errorMsg);

    void error(const char *format, ...);

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
