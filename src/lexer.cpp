#include "lexer.h"
#include <iostream>

namespace lexer {
    std::vector <parser::Token> Lexer::scan(const std::string &code) const {
//        std::cout << "start scan " << code << std::endl;
        std::vector <parser::Token> tokens;
        int begin = 0, lastTok = 0, cur = 0;
        int len = code.size();
        parser::TokenType lastType = parser::INVALID;
        std::string lex;
        while (cur < len) {
            lex += code[cur];
//            std::cout << "cur lex: " << lex << std::endl;
            auto tokenType = match(lex);
            if (tokenType != parser::INVALID) {
                lastTok = cur;
                lastType = tokenType;
                ++cur;
            } else {
                std::string token = code.substr(begin, lastTok - begin + 1);
                if (lastType == parser::INVALID) throw "Invalid tokens!";
//                std::cout << "read token: " << token << std::endl;
                if (lastType != parser::BLANK) {
                    tokens.push_back(parser::Token::makeToken(token, lastType));
                }
                lastTok = cur = begin = lastTok + 1;
                lex.clear();
            }
        }
//        std::cout << lastTok << " " << begin << std::endl;
        if (lastType == parser::INVALID) throw "Invalid tokens!";
        if (lastTok >= begin && lastTok < len) {
            std::string token = code.substr(begin, lastTok - begin + 1);
//            std::cout << "read token: " << token << std::endl;
            tokens.push_back(parser::Token::makeToken(token, lastType));
        }
        return tokens;
    }

    const std::string Lexer::blankFmt = "\\s+";
    const std::string Lexer::letFmt = "LET";
    const std::string Lexer::ifFmt = "IF";
    const std::string Lexer::thenFmt = "THEN";
    const std::string Lexer::gotoFmt = "GOTO";
    const std::string Lexer::printFmt = "PRINT";
    const std::string Lexer::remFmt = "REM.*";
    const std::string Lexer::endFmt = "END";
    const std::string Lexer::inputFmt = "INPUT";
    const std::string Lexer::idFmt = "[a-zA-Z][a-zA-Z0-9]*";
    const std::string Lexer::intFmt = "(\\-)?[0-9]+";
    const std::string Lexer::eqFmt = "=";
    const std::string Lexer::ltFmt = "\\<";
    const std::string Lexer::leFmt = "\\<=";
    const std::string Lexer::neqFmt = "\\<\\>";
    const std::string Lexer::gtFmt = "\\>";
    const std::string Lexer::geFmt = "\\>=";
    const std::string Lexer::plusFmt = "\\+";
    const std::string Lexer::minusFmt = "\\-";
    const std::string Lexer::timesFmt = "\\*";
    const std::string Lexer::divideFmt = "/";
    const std::string Lexer::indexFmt = "\\*\\*";
    const std::string Lexer::lparenFmt = "\\(";
    const std::string Lexer::rparenFmt = "\\)";

    std::vector <std::pair<std::string, parser::TokenType>> Lexer::fmtAndType = {
            {blankFmt,  parser::BLANK},
            {letFmt,    parser::LET},
            {ifFmt,     parser::IF},
            {thenFmt,   parser::THEN},
            {gotoFmt,   parser::GOTO},
            {printFmt,  parser::PRINT},
            {remFmt,    parser::REM},
            {endFmt,    parser::END},
            {inputFmt,  parser::INPUT},
            {idFmt,     parser::ID},
            {intFmt,    parser::INT},
            {eqFmt,     parser::EQ},
            {ltFmt,     parser::LT},
            {leFmt,     parser::LE},
            {neqFmt,    parser::NEQ},
            {gtFmt,     parser::GT},
            {geFmt,     parser::GE},
            {plusFmt,   parser::PLUS},
            {minusFmt,  parser::MINUS},
            {timesFmt,  parser::TIMES},
            {divideFmt, parser::DIVIDE},
            {indexFmt,  parser::INDEX},
            {lparenFmt, parser::LPAREN},
            {rparenFmt, parser::RPAREN},
    };
}
