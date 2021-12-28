#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <regex>
#include <utility>
#include <vector>
#include "parser.h"

namespace lexer {
    class Lexer {
    public:
        Lexer() = default;

        std::vector <parser::Token> scan(const std::string &code) const;

    private:
        static inline bool isMatched(const std::string &fmt, const std::string &lex) {
            return std::regex_match(lex, std::regex(fmt));
        }

        static parser::Token matchLongest(const std::string &code, int start);

        static inline parser::TokenType match(const std::string &lex) {
            for (const auto &fmtAndTy: fmtAndType) {
                if (isMatched(fmtAndTy.first, lex)) {
                    return fmtAndTy.second;
                }
            }
            return parser::INVALID;
        }

        static std::vector <std::pair<std::string, parser::TokenType>> fmtAndType;

        static const std::string blankFmt;
        static const std::string letFmt;
        static const std::string ifFmt;
        static const std::string thenFmt;
        static const std::string gotoFmt;
        static const std::string printFmt;
        static const std::string remFmt;
        static const std::string endFmt;
        static const std::string inputFmt;
        static const std::string idFmt;
        static const std::string intFmt;
        static const std::string eqFmt;
        static const std::string ltFmt;
        static const std::string leFmt;
        static const std::string neqFmt;
        static const std::string gtFmt;
        static const std::string geFmt;
        static const std::string plusFmt;
        static const std::string minusFmt;
        static const std::string timesFmt;
        static const std::string divideFmt;
        static const std::string indexFmt;
        static const std::string lparenFmt;
        static const std::string rparenFmt;
    };
}


#endif // LEXER_H
