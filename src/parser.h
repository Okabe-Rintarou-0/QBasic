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
    };

    class ParseException : public std::exception {
    private:
        std::string msg;
    public:
        ParseException(const std::string &msg) : msg(msg) {};

        ~ParseException() = default;

        virtual const char *what(void) const noexcept override;
    };
}


#endif // PARSER_H
