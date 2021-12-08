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

    enum ArithmeticOp {
        PLUS_OP,
        MINUS_OP,
        TIMES_OP,
        DIVIDE_OP,
        INDEX_OP
    };

    enum LogicOp {
        EQ,
        NEQ,
        GT,
        GE,
        LT,
        LE
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
        Exp *exp;

    public:
        PrintExp(Exp *exp) : exp(exp) {}

        inline void clear() override {
            exp->clear();
            delete exp;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "PRINT\n";
            exp->print(str, depth + 1);
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

    class ArithmeticExp : public Exp {
    private:
        ArithmeticOp op;
        Exp *left, *right;

        ArithmeticExp(ArithmeticOp op, Exp *left, Exp *right) : op(op), left(left), right(right) {}

    public:
        static inline ArithmeticExp *plusExp(Exp *left, Exp *right) {
            return new ArithmeticExp(PLUS_OP, left, right);
        }

        static inline ArithmeticExp *minusExp(Exp *left, Exp *right) {
            return new ArithmeticExp(MINUS_OP, left, right);
        }

        static inline ArithmeticExp *timesExp(Exp *left, Exp *right) {
            return new ArithmeticExp(TIMES_OP, left, right);
        }

        static inline ArithmeticExp *divideExp(Exp *left, Exp *right) {
            return new ArithmeticExp(DIVIDE_OP, left, right);
        }

        static inline ArithmeticExp *indexExp(Exp *left, Exp *right) {
            return new ArithmeticExp(INDEX_OP, left, right);
        }

        inline void clear() override {
            left->clear();
            right->clear();
            delete left;
            delete right;
        }

        inline void print(std::string &str, int depth) override;

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class LetExp : public Exp {
    private:
        VarExp *var;
        Exp *val;

    public:
        LetExp(VarExp *var, Exp *val) : var(var), val(val) {}

        inline void clear() override {
            var->clear();
            val->clear();
            delete var;
            delete val;
        }

        inline void print(std::string &str, int depth) override {
            indent(str, depth);
            str += "LET =\n";
            var->print(str, depth + 1);
            str += '\n';
            val->print(str, depth + 1);
        }

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class LogicalExp : public Exp {
    private:
        LogicOp op;
        Exp *left, *right;
    public:
        LogicalExp(LogicOp op, Exp *left, Exp *right) : op(op), left(left), right(right) {}

        static inline LogicalExp *eqExp(Exp *left, Exp *right) {
            return new LogicalExp(EQ, left, right);
        }

        static inline LogicalExp *neqExp(Exp *left, Exp *right) {
            return new LogicalExp(NEQ, left, right);
        }

        static inline LogicalExp *gtExp(Exp *left, Exp *right) {
            return new LogicalExp(GT, left, right);
        }

        static inline LogicalExp *geExp(Exp *left, Exp *right) {
            return new LogicalExp(GE, left, right);
        }

        static inline LogicalExp *ltExp(Exp *left, Exp *right) {
            return new LogicalExp(LT, left, right);
        }

        static inline LogicalExp *leExp(Exp *left, Exp *right) {
            return new LogicalExp(LE, left, right);
        }

        inline void clear() override {
            left->clear();
            right->clear();
            delete left;
            delete right;
        }

        void print(std::string &str, int depth) override;

        ExpVal run(MainWindow *mainWindow, QTextBrowser *resultDisplay) override;
    };

    class IfThenExp : public Exp {
    private:
        LogicalExp *test;
        IntExp *lineno;
    public:
        IfThenExp(LogicalExp *test, IntExp *lineno) : test(test), lineno(lineno) {}

        inline void clear() override {
            test->clear();
            lineno->clear();
            delete test;
            delete lineno;
        }

        void print(std::string &str, int depth) override;

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
