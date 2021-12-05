#include "parser.h"
#include "stringutils.h"
#include "statement.h"

namespace parser {
    statement::StatementType Parser::getStatementType(const std::vector <Token> &tokens, std::string &errorMsg) {
        if (tokens.empty()) {
            errorMsg = "Statement can't be empty!";
            return statement::STMT_INVALID;
        }

        if (tokens[0].type == REM)
            return statement::STMT_REM;
    }

    statement::Statement *Parser::parse(int lineno, const std::string &srcCode, const std::vector <Token> &tokens) {
        std::string errorMsg;
        auto stmtType = getStatementType(tokens, errorMsg);
        if (stmtType == statement::STMT_INVALID) {
            throw new ParseException(errorMsg);
        }
        statement::Statement *statement = nullptr;
        switch (stmtType) {
            case statement::STMT_REM: {
                std::string content = StringUtils::getAfter(tokens[0].tok, "REM");
                std::cout << "content: " << content << std::endl;
                syntax::Exp *exp = new syntax::RemExp(new syntax::StringExp(content));
                statement = new statement::RemStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
            }
            default:
                break;
        }
        return statement;
    }

    const char *ParseException::what() const noexcept {
        return msg.c_str();
    }
}
