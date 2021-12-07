#include "parser.h"
#include "stringutils.h"

namespace parser {
    statement::StatementType Parser::getStatementType(const std::vector <Token> &tokens, std::string &errorMsg) {
        if (tokens.empty()) {
            errorMsg = "Statement can't be empty!";
            throw errorMsg;
        }

        if (tokens[0].type == REM && tokens.size() == 1) {
            return statement::STMT_REM;
        }

        if (tokens[0].type == END && tokens.size() == 1) {
            return statement::STMT_END;
        }

        if (tokens[0].type == INPUT && tokens.size() == 2 && tokens[1].type == ID) {
            return statement::STMT_INPUT;
        }

        if (tokens[0].type == PRINT && tokens.size() == 2 && tokens[1].type == ID) {
            return statement::STMT_PRINT;
        }

        if (tokens[0].type == GOTO && tokens.size() == 2 && tokens[1].type == INT) {
            return statement::STMT_GOTO;
        }

        return statement::STMT_INVALID;
    }

    statement::Statement *Parser::parse(int lineno, const std::string &srcCode, const std::vector <Token> &tokens) {
        std::string errorMsg;
        auto stmtType = getStatementType(tokens, errorMsg);
        if (stmtType == statement::STMT_INVALID) {
            throw errorMsg;
        }
        statement::Statement *statement = nullptr;
        syntax::Exp *exp = nullptr;
        switch (stmtType) {
            case statement::STMT_REM: {
                std::string content = StringUtils::getAfter(tokens[0].tok, "REM");
                exp = new syntax::RemExp(new syntax::StringExp(content));
                statement = new statement::RemStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_PRINT: {
                std::string var = tokens[1].tok;
                exp = new syntax::PrintExp(new syntax::VarExp(var));
                statement = new statement::PrintStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_INPUT: {
                std::string var = tokens[1].tok;
                exp = new syntax::InputExp(new syntax::VarExp(var));
                statement = new statement::InputStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_GOTO: {
                int lineno = std::atoi(tokens[1].tok.c_str());
                exp = new syntax::GotoExp(new syntax::IntExp(lineno));
                statement = new statement::GotoStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_END: {
                exp = new syntax::EndExp;
                statement = new statement::EndStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            default:
                break;
        }
        return statement;
    }
}
