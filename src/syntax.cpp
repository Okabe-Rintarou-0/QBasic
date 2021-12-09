#include "syntax.h"
#include <cmath>

namespace syntax {
    void PrintExp::checkValidation(MainWindow *mainWindow) {
        exp->checkValidation(mainWindow);
    }

    ExpVal PrintExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        Q_UNUSED(mainWindow);
        ExpVal varVal = exp->run(mainWindow, resultDisplay);
        if (varVal.type == INT)
            resultDisplay->append(QString::number(varVal.iVal));
        else if (varVal.type == STRING)
            resultDisplay->append(QString::fromStdString(varVal.sVal));
        else
            throw "Use undefined variable!";
        return ExpVal::voidValue();
    }

    ExpVal InputExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        Q_UNUSED(resultDisplay);
        mainWindow->input(var->getSymbol());
        return ExpVal::voidValue();
    }

    ExpVal GotoExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        Q_UNUSED(resultDisplay);
        mainWindow->gotoLine(lineno->getValue());
        return ExpVal::voidValue();
    }

    ExpVal EndExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        Q_UNUSED(resultDisplay);
        mainWindow->end();
        return ExpVal::voidValue();
    }

    void ArithmeticExp::checkValidation(MainWindow *mainWindow) {
        left->checkValidation(mainWindow);
        right->checkValidation(mainWindow);
        ExpVal leftVal = left->run(mainWindow, nullptr);
        ExpVal rightVal = right->run(mainWindow, nullptr);
        std::cout << "check: " << rightVal.iVal << std::endl;
        if (leftVal.type != INT || rightVal.type != INT)
            throw "Arithmetic operation only supports int!";
//            std::cout << "check: " << rightVal.iVal << std::endl;
        if (op == DIVIDE_OP && rightVal.iVal == 0)
            throw "Divided by zero!";
    }

    void ArithmeticExp::print(std::string &str, int depth) {
        indent(str, depth);
        switch (op) {
            case PLUS_OP:
                str += '+';
                break;
            case MINUS_OP:
                str += '-';
                break;
            case TIMES_OP:
                str += '*';
                break;
            case DIVIDE_OP:
                str += '/';
                break;
            case INDEX_OP:
                str += "**";
                break;
        }
        str += '\n';
        left->print(str, depth + 1);
        str += '\n';
        right->print(str, depth + 1);
    }

    ExpVal ArithmeticExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        ExpVal leftVal = left->run(mainWindow, resultDisplay);
        ExpVal rightVal = right->run(mainWindow, resultDisplay);
        if (leftVal.type != INT || rightVal.type != INT)
            throw "Arithmetic operation only supports int!";

        switch (op) {
            case PLUS_OP:
                return ExpVal(leftVal.iVal + rightVal.iVal);
            case MINUS_OP:
                return ExpVal(leftVal.iVal - rightVal.iVal);
            case TIMES_OP:
                return ExpVal(leftVal.iVal * rightVal.iVal);
            case DIVIDE_OP:
                if (rightVal.iVal == 0) throw "Divided by zero!";
                return ExpVal(leftVal.iVal / rightVal.iVal);
            case INDEX_OP:
                return ExpVal(int(pow(leftVal.iVal, rightVal.iVal)));
            default:
                break;
        }
        throw "Non-existent operation type!";
    }

    void LetExp::checkValidation(MainWindow *mainWindow) {
        if (typeid(*val) != typeid(StringExp) && typeid(*val) != typeid(IntExp) &&
            typeid(*val) != typeid(ArithmeticExp))
            throw "Invalid assignment value!";
        var->checkValidation(mainWindow);
        val->checkValidation(mainWindow);
    }

    ExpVal LetExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        if (typeid(*val) != typeid(StringExp) && typeid(*val) != typeid(IntExp) &&
            typeid(*val) != typeid(ArithmeticExp))
            throw "Invalid assignment value!";

        ExpVal expVal = val->run(mainWindow, resultDisplay);
        std::string varSymbol = var->getSymbol();

        if (expVal.type == INT) {
            mainWindow->tenv->enter(varSymbol, env::INT);
            mainWindow->venv->enter(varSymbol, expVal.iVal);
        } else {
            mainWindow->tenv->enter(varSymbol, env::STRING);
            mainWindow->venv->enter(varSymbol, expVal.sVal);
        }

        return ExpVal::voidValue();
    }

    void LogicalExp::checkValidation(MainWindow *mainWindow) {
        left->checkValidation(mainWindow);
        right->checkValidation(mainWindow);
        ExpVal leftVal = left->run(mainWindow, nullptr);
        ExpVal rightVal = right->run(mainWindow, nullptr);
        if (leftVal.type != INT || rightVal.type != INT)
            throw "Logical operation only supports int!";
    }

    void LogicalExp::print(std::string &str, int depth) {
        indent(str, depth);
        switch (op) {
            case EQ:
                str += '=';
                break;
            case NEQ:
                str += "<>";
                break;
            case GT:
                str += '>';
                break;
            case GE:
                str += ">=";
                break;
            case LT:
                str += '<';
                break;
            case LE:
                str += "<=";
                break;
        }
        str += '\n';
        left->print(str, depth);
        str += '\n';
        right->print(str, depth);
    }

    ExpVal LogicalExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        ExpVal leftVal = left->run(mainWindow, resultDisplay);
        ExpVal rightVal = right->run(mainWindow, resultDisplay);
        if (leftVal.type != INT || rightVal.type != INT)
            throw "Logical operation only supports int!";

        bool _true = false;
        switch (op) {
            case EQ:
                _true = leftVal.iVal == rightVal.iVal;
                break;
            case NEQ:
                _true = leftVal.iVal != rightVal.iVal;
                break;
            case GT:
                _true = leftVal.iVal > rightVal.iVal;
                break;
            case GE:
                _true = leftVal.iVal >= rightVal.iVal;
                break;
            case LT:
                _true = leftVal.iVal < rightVal.iVal;
                break;
            case LE:
                _true = leftVal.iVal <= rightVal.iVal;
                break;
        }
        return ExpVal(int(_true));
    }

    void IfThenExp::checkValidation(MainWindow *mainWindow) {
        test->checkValidation(mainWindow);
        if (lineno->getValue() <= 0 || lineno->getValue() > 100000)
            throw "Invalid line number!";
    }

    void IfThenExp::print(std::string &str, int depth) {
        indent(str, depth);
        str += "IF THEN\n";
        test->print(str, depth + 1);
        str += '\n';
        lineno->print(str, depth + 1);
    }

    ExpVal IfThenExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        ExpVal testVal = test->run(mainWindow, resultDisplay);
        ExpVal linenoVal = lineno->run(mainWindow, resultDisplay);
        if (testVal.iVal == 1) {
            mainWindow->gotoLine(linenoVal.iVal);
        }

        return ExpVal::voidValue();
    }

    ExpVal ExpVal::_voidVal(true);

    SyntaxTree::SyntaxTree(Exp *root) : root(root) {}

    SyntaxTree::~SyntaxTree() { clear(); }
}
