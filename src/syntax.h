#ifndef QBASIC_SYNTAX_H
#define QBASIC_SYNTAX_H

#include <iostream>
#include <string>
#include "mainwindow.h"
#include <QTextBrowser>

namespace syntax {
    inline void indent(std::string &str, int depth) {
        for (int i = 0; i < depth; ++i)
            str += "  ";
    }

    class ExpVal;

    class Exp;

    enum valueType {
        INT,
        STRING,
        VOID
    };

    class ExpVal {
    public:
        ExpVal() = default;

        ExpVal(int iVal) : iVal(iVal), type(INT) {}

        ExpVal(const std::string &sVal) : sVal(sVal), type(STRING) {}

        static inline ExpVal voidValue() { return _voidVal; }

        int iVal;
        std::string sVal;
        valueType type;
    private:
        ExpVal(bool isVoid) { if (isVoid) type = VOID; }

        static ExpVal _voidVal;
    };

    class Exp {
    public:
        virtual void print(std::string &str, int depth) = 0;

        virtual ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) = 0;

        virtual void clear() = 0;

        virtual ~Exp() = default;
    };

    class StringExp : public Exp {
    private:
        std::string val;

    public:
        StringExp(const std::string &val) : val(val) {}

        inline void clear() override {
            return;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += this->val;
        };

        inline ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override {
            Q_UNUSED(mainWindow);
            Q_UNUSED(resultDisplay);
            return ExpVal(this->val);
        }
    };

    class IntExp : public Exp {
    private:
        int val;

    public:
        IntExp(int val) : val(val) {}

        inline void clear() override {
            return;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += std::to_string(val);
        };

        inline ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override {
            Q_UNUSED(mainWindow);
            Q_UNUSED(resultDisplay);
            return ExpVal(this->val);
        }

        inline int getValue() const { return val; }
    };

    class RemExp : public Exp {
    private:
        StringExp *content;

    public:
        RemExp(StringExp *content) : content(content) {}

        inline void clear() override {
            content->clear();
            delete content;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "REM\n";
            content->print(str, depth + 1);
        }

        inline ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override {
            Q_UNUSED(mainWindow);
            Q_UNUSED(resultDisplay);
            return ExpVal::voidValue();
        }
    };

    class VarExp : public Exp {
    private:
        std::string symbol;

    public:
        VarExp(const std::string &symbol) : symbol(symbol) {}

        inline void clear() override {}

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += this->symbol;
        }

        inline ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override {
            Q_UNUSED(resultDisplay);
            env::ValueType *valueTypePtr = mainWindow->tenv->look(symbol);
            if (valueTypePtr != nullptr) {
                env::ValueType valueType = *valueTypePtr;
                env::Value *value = mainWindow->venv->look(symbol);
                if (valueType == env::INT)
                    return ExpVal(value->getInt());
                else
                    return ExpVal(value->getString());
            }
            return ExpVal::voidValue();
        }

        inline std::string getSymbol() const { return symbol; }
    };

    class PrintExp : public Exp {
    private:
        VarExp *var;

    public:
        PrintExp(VarExp *var) : var(var) {}

        inline void clear() override {
            var->clear();
            delete var;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "PRINT\n";
            var->print(str, depth + 1);
        }

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class InputExp : public Exp {
    private:
        VarExp *var;
    public:
        InputExp(VarExp *var) : var(var) {}

        inline void clear() override {
            var->clear();
            delete var;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "INPUT\n";
            var->print(str, depth + 1);
        }

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class GotoExp : public Exp {
    private:
        IntExp *lineno;
    public:
        GotoExp(IntExp *lineno) : lineno(lineno) {}

        inline void clear() override {
            lineno->clear();
            delete lineno;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "GOTO\n";
            lineno->print(str, depth + 1);
        }

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class EndExp : public Exp {
    public:
        EndExp() = default;

        inline void clear() override {}

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "END";
        }

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class SyntaxTree {
    public:
        SyntaxTree() = delete;

        ~SyntaxTree();

        SyntaxTree(Exp *root);

        inline void print(std::string &str) {
            root->print(str, 0);
            str += '\n';
        }

        inline void run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
            root->run(mainWindow, resultDisplay);
        }

        inline void clear() {
            root->clear();
            delete root;
        }

    private:
        Exp *root;
    };
}


#endif //QBASIC_SYNTAX_H
