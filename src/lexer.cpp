#include "lexer.h"
#include <iostream>

namespace lexer {
    parser::Token Lexer::matchLongest(const std::string &code, int start) {
        std::string lex;
        int matched_idx = start;
        parser::TokenType matchedType;
        int len = code.size();
        for (int i = start; i < len; ++i) {
            lex += code[i];
            parser::TokenType thisType = match(lex);
            if (thisType != parser::INVALID) {
                matched_idx = i + 1;
                matchedType = thisType;
            }
        }
        if (matched_idx == start) {
            return parser::Token(code.substr(start, 1), parser::INVALID);
        } else {
            return parser::Token(code.substr(start, matched_idx - start), matchedType);
        }
    }

    std::vector <parser::Token> Lexer::scan(const std::string &code) const {
        std::vector <parser::Token> tokens;
        int cur = 0;
        int len = code.size();
        while (cur < len) {
            parser::Token token = matchLongest(code, cur);
            parser::TokenType type = token.type;
            if (type == parser::INVALID)
                throw "Invalid tokens!";

            cur += token.tok.size();
            if (type != parser::BLANK) {
                // some special judge, to handle with negative number
                if (type == parser::INT) {
                    if (token.tok[0] == '(') { // remove the parenthesis
                        token.tok = token.tok.substr(1, token.tok.size() - 2);
                    } else {
                        int len = tokens.size();
                        if (len > 2 && tokens[len - 1].type == parser::MINUS &&
                            (tokens[len - 2].type == parser::PRINT || tokens[len - 2].type == parser::EQ)) {
                            // special judge, the negative number at the beginning
                            tokens[len - 1].type = parser::INT;
                            tokens[len - 1].tok += token.tok;
                            continue;
                        }
                    }
                }
                tokens.push_back(token);
            }
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
    const std::string Lexer::intFmt = "[0-9]+|(\\((\\-)?[0-9]+\\))";
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
