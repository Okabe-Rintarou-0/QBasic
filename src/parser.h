#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <exception>
#include "statement.h"

namespace parser {
    enum TokenType {
        INVALID = -1,
        BLANK,
        LET,
        IF,
        THEN,
        GOTO,
        PRINT,
        REM,
        END,
        INPUT,
        ID,
        INT,
        EQ,
        LT,
        LE,
        NEQ,
        GT,
        GE,
        PLUS,
        MINUS,
        TIMES,
        DIVIDE,
        INDEX,
        LPAREN,
        RPAREN,
    };

    static std::unordered_map <TokenType, std::string> typeTable = {
            {INVALID, "INVALID"},
            {LET,     "LET"},
            {IF,      "IF"},
            {THEN,    "THEN"},
            {GOTO,    "GOTO"},
            {PRINT,   "PRINT"},
            {REM,     "REM"},
            {END,     "END"},
            {INPUT,   "INPUT"},
            {ID,      "ID"},
            {INT,     "INT"},
            {EQ,      "EQ"},
            {LT,      "LT"},
            {LE,      "LE"},
            {NEQ,     "NEQ"},
            {GT,      "GT"},
            {GE,      "GE"},
            {PLUS,    "PLUS"},
            {MINUS,   "MINUS"},
            {TIMES,   "TIMES"},
            {DIVIDE,  "DIVIDE"},
            {INDEX,   "INDEX"},
            {LPAREN,  "LPAREN"},
            {RPAREN,  "RPAREN"},
    };

    static std::unordered_map<TokenType, int> prior = {
            {INVALID, 0},
            {LPAREN,  1},
            {PLUS,    2},
            {MINUS,   2},
            {TIMES,   3},
            {DIVIDE,  3},
            {INDEX,   4}
    };

    class Token {
    public:
        std::string tok;
        TokenType type;

        Token(const std::string &tok, TokenType type) : tok(tok), type(type) {}

        static Token makeToken(const std::string &tok, TokenType type) {
            return Token(tok, type);
        }
    };

    inline std::ostream &operator<<(std::ostream &os, const Token &token) {
        return os << "(" << token.tok << ", " << typeTable[token.type] << ")";
    }

    class Parser {
    public:
        Parser() = default;

        statement::StatementType getStatementType(const std::vector <Token> &tokens, std::string &errorMsg);

        statement::Statement *parse(int lineno, const std::string &srcCode, const std::vector <Token> &tokens);

    private:
        syntax::Exp *parseArithmetic(const std::vector <Token> &tokens) const;

        syntax::LogicalExp *parseLogical(const std::vector <Token> &tokens) const;

        // RPN namely reverse polish notation.
        void computeRPN(const std::vector <Token> &tokens, std::vector <Token> &rpn) const;
    };
}


#endif // PARSER_H
