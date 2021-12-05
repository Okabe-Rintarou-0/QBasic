#ifndef QBASIC_SYNTAX_H
#define QBASIC_SYNTAX_H

#include <iostream>
#include <string>

namespace syntax {
inline void indent(std::string &str, int depth) {
    for(int i = 0; i < depth; ++i)
        str += "  ";
}

    class ExpVal;

    class Exp;

    class ExpVal {
    public:
        ExpVal() = default;

        ExpVal(int iVal) : iVal(iVal) {}

        ExpVal(const std::string &sVal) : sVal(sVal) {}

        int iVal;
        std::string sVal;
    };

    class Exp {
    public:
        virtual void print(std::string &str, int depth) = 0;

        virtual ExpVal run() = 0;

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

        inline ExpVal run() override {
            return ExpVal(this->val);
        }
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

        inline ExpVal run() override {
            return ExpVal();
        }

        inline void print(std::string &str, int depth) override {
            str += "REM\n";
            content->print(str, depth + 1);
        }
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

        inline void run() {
            root->run();
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
