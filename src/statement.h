#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>
#include <regex>
#include <QMainWindow>
#include "syntax.h"

using SyntaxTree = syntax::SyntaxTree;
namespace statement {
    enum StatementType {
        STMT_INVALID = -1,
        STMT_LET,
        STMT_REM,
        STMT_IF_THEN,
        STMT_PRINT,
        STMT_INPUT,
        STMT_GOTO,
        STMT_END,
    };

    class RawStatement {
    public:
        RawStatement(int lineno, const std::string &srcCode) : lineno(lineno), srcCode(srcCode) {};

        RawStatement() = delete;

        ~RawStatement() = default;

        static RawStatement *fromCmdline(const std::string &cmdline);

        inline std::string toString() const {
            return std::to_string(lineno) + " " + srcCode;
        }

        inline static bool valid(const std::string &cmdline) {
            static std::regex pattern("^[1-9]\\d* .*$");
            return std::regex_match(cmdline, pattern);
        }

        int lineno;
        std::string srcCode;
    };

    class Statement {
    public:
        Statement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : lineno(lineno), srcCode(srcCode),
                                                                                    syntaxTree(syntaxTree) {};

        virtual ~Statement() = default;

        inline int getLineno() const {
            return lineno;
        }

        inline std::string getSrcCode() const {
            return srcCode;
        }

        inline std::string toString() const {
            return std::to_string(lineno) + " " + srcCode;
        }

        virtual void run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
            syntaxTree->run(mainWindow, resultDisplay);
        }

        virtual void clear() {
            syntaxTree->clear();
            delete syntaxTree;
        }

        virtual inline void print(std::string &str) {
            str += std::to_string(lineno) + ' ';
            syntaxTree->print(str);
        }

    protected:
        int lineno;
        std::string srcCode;

        SyntaxTree *syntaxTree;
    };

    class ErrorStatement : public Statement {
    public:
        ErrorStatement(int lineno) : Statement(lineno, "", nullptr) {};

        inline void clear() override {}

        inline void print(std::string &str) override {
            str += std::to_string(lineno) + " Error\n";
        }

        inline void run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override {
            Q_UNUSED(mainWindow);
            Q_UNUSED(resultDisplay);
        }
    };

    class RemStatement : public Statement {
    public:

        RemStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                 syntaxTree) {}

        ~RemStatement() = default;
    };

    class PrintStatement : public Statement {
    public:

        PrintStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                   syntaxTree) {}

        ~PrintStatement() = default;
    };

    class InputStatement : public Statement {
    public:

        InputStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                   syntaxTree) {}

        ~InputStatement() = default;
    };

    class GotoStatement : public Statement {
    public:

        GotoStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                  syntaxTree) {}

        ~GotoStatement() = default;
    };

    class LetStatement : public Statement {
    public:

        LetStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                 syntaxTree) {}

        ~LetStatement() = default;
    };

    class IfThenStatement : public Statement {
    public:

        IfThenStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                    syntaxTree) {}

        ~IfThenStatement() = default;
    };

    class EndStatement : public Statement {
    public:

        EndStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                 syntaxTree) {}

        ~EndStatement() = default;
    };
}

#endif // STATEMENT_H
