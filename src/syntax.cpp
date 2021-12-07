#include "syntax.h"

namespace syntax {
    ExpVal PrintExp::run(MainWindow *mainWindow, QTextBrowser *resultDisplay) {
        Q_UNUSED(mainWindow);
        ExpVal varVal = var->run(mainWindow, resultDisplay);
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

    ExpVal ExpVal::_voidVal(true);

    SyntaxTree::SyntaxTree(Exp *root) : root(root) {}

    SyntaxTree::~SyntaxTree() { clear(); }
}
