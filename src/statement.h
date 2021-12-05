#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>
#include <regex>
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
        RawStatement(int lineno, const std::string &srcCode): lineno(lineno), srcCode(srcCode){};

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
        Statement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree):lineno(lineno), srcCode(srcCode), syntaxTree(syntaxTree){};

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

        virtual void run() {
            syntaxTree->run();
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

    class RemStatement : public Statement {
    public:

        RemStatement(int lineno, const std::string &srcCode, SyntaxTree *syntaxTree) : Statement(lineno, srcCode,
                                                                                                 syntaxTree) {}

        ~RemStatement() = default;
    };
}

#endif // STATEMENT_H
