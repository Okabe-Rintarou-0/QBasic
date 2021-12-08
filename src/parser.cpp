#include "parser.h"
#include "stringutils.h"
#include <stack>

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

        if (tokens[0].type == PRINT && tokens.size() >= 2) {
            return statement::STMT_PRINT;
        }

        if (tokens[0].type == GOTO && tokens.size() == 2 && tokens[1].type == INT) {
            return statement::STMT_GOTO;
        }

        if (tokens[0].type == LET && tokens.size() >= 4 && tokens[1].type == ID && tokens[2].type == EQ) {
            return statement::STMT_LET;
        }

        if (tokens[0].type == IF && tokens.size() >= 6 && tokens[tokens.size() - 2].type == THEN &&
            tokens[tokens.size() - 1].type == INT) {
            return statement::STMT_IF_THEN;
        }

        return statement::STMT_INVALID;
    }

    void Parser::computeRPN(const std::vector <Token> &tokens, std::vector <Token> &rpn) const {
        std::stack <Token> opStack;
        Token lastToken = Token::makeToken("", INVALID);
        opStack.push(Token::makeToken("", INVALID));
        for (const Token &token:tokens) {
            TokenType lastType = lastToken.type;
            switch (token.type) {
                case ID:
                case INT: {
                    if (lastType == ID || lastType == INT || lastType == RPAREN)
                        throw "Invalid exp!";
                    rpn.push_back(token);
                    break;
                }
                case PLUS:
                case MINUS:
                case TIMES:
                case DIVIDE:
                case INDEX: {
                    if (lastType != RPAREN && lastType != ID && lastType != INT)
                        throw "Invalid exp!";
                    int thisPrior = prior[token.type];
                    while (thisPrior <= prior[opStack.top().type]) {
                        rpn.push_back(opStack.top());
                        opStack.pop();
                    }
                    opStack.push(token);
                    break;
                }
                case LPAREN: {
                    opStack.push(token);
                    break;
                }
                case RPAREN: {
                    while (opStack.size() > 1 && opStack.top().type != LPAREN) {
                        rpn.push_back(opStack.top());
                        opStack.pop();
                    }
                    // pop lparen
                    if (opStack.size() > 1) {
                        opStack.pop();
                    } else {
                        throw "Parenthesis not match!";
                    }
                    break;
                }
                default:
                    throw "Invalid exp!";
            }
            lastToken = token;
        }

        while (opStack.size() > 1) {
            if (opStack.top().type == LPAREN) throw "Parenthesis not match!";
            rpn.push_back(opStack.top());
            opStack.pop();
        }
    }

    syntax::Exp *Parser::parseArithmetic(const std::vector <Token> &tokens) const {
        std::vector <Token> rpn;
        std::stack < syntax::Exp * > expStack;
        computeRPN(tokens, rpn);

        std::cout << "rpn" << std::endl;
        for (auto token: rpn) {
            std::cout << token << std::endl;
        }
        for (auto token:rpn) {
            switch (token.type) {
                case INT: {
                    expStack.push(new syntax::IntExp(std::atoi(token.tok.c_str())));
                    break;
                }
                case ID: {
                    expStack.push(new syntax::VarExp(token.tok));
                    break;
                }
                case PLUS:
                case MINUS:
                case TIMES:
                case DIVIDE:
                case INDEX: {
                    if (expStack.size() < 2) throw "Invalid exp!";
                    syntax::Exp *right = expStack.top();
                    expStack.pop();
                    syntax::Exp *left = expStack.top();
                    expStack.pop();

                    syntax::Exp *arithmetic = nullptr;

                    switch (token.type) {
                        case PLUS:
                            arithmetic = syntax::ArithmeticExp::plusExp(left, right);
                            break;
                        case MINUS:
                            arithmetic = syntax::ArithmeticExp::minusExp(left, right);
                            break;
                        case TIMES:
                            arithmetic = syntax::ArithmeticExp::timesExp(left, right);
                            break;
                        case DIVIDE:
                            arithmetic = syntax::ArithmeticExp::divideExp(left, right);
                            break;
                        case INDEX:
                            arithmetic = syntax::ArithmeticExp::indexExp(left, right);
                            break;
                        default:
                            break;
                    }
                    expStack.push(arithmetic);
                    break;
                }
                default:
                    break;
            }
        }

        if (expStack.size() != 1) throw "Invalid exp!";

        return expStack.top();
    }

    syntax::LogicalExp *Parser::parseLogical(const std::vector <Token> &tokens) const {
        int idx = -1;
        int len = tokens.size();
        for (int i = 0; i < len; ++i) {
            TokenType type = tokens[i].type;
            if (type == EQ || type == NEQ || type == GT || type == GE || type == LT || type == LE) {
                if (idx != -1) throw "Invalid if then exp!";
                idx = i;
            }
        }
        if (idx == -1) throw "Invalid if then exp!";

        std::vector <Token> leftTokens(tokens.begin(), tokens.begin() + idx);
        std::vector <Token> rightTokens(tokens.begin() + idx + 1, tokens.end());

        syntax::Exp *left = parseArithmetic(leftTokens);
        syntax::Exp *right = parseArithmetic(rightTokens);

        std::cout << "logical op: " << tokens[idx] << std::endl;

        switch (tokens[idx].type) {
            case EQ:
                return syntax::LogicalExp::eqExp(left, right);
            case NEQ:
                return syntax::LogicalExp::neqExp(left, right);
            case GT:
                return syntax::LogicalExp::gtExp(left, right);
            case GE:
                return syntax::LogicalExp::geExp(left, right);
            case LT:
                return syntax::LogicalExp::ltExp(left, right);
            case LE:
                return syntax::LogicalExp::leExp(left, right);
            default:
                break;
        }

        return nullptr;
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
                std::vector <Token> expTokens(tokens.begin() + 1, tokens.end());
                exp = new syntax::PrintExp(parseArithmetic(expTokens));
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
                int tgtLineno = std::atoi(tokens[1].tok.c_str());
                exp = new syntax::GotoExp(new syntax::IntExp(tgtLineno));
                statement = new statement::GotoStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_END: {
                exp = new syntax::EndExp;
                statement = new statement::EndStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_LET: {
                std::string var = tokens[1].tok;
                std::vector <Token> expTokens(tokens.begin() + 3, tokens.end());
                exp = new syntax::LetExp(new syntax::VarExp(var), parseArithmetic(expTokens));
                statement = new statement::LetStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            case statement::STMT_IF_THEN: {
                int len = tokens.size();
                int tgtLineno = std::atoi(tokens[len - 1].tok.c_str());
                std::vector <Token> expTokens(tokens.begin() + 1, tokens.begin() + (len - 2));
                exp = new syntax::IfThenExp(parseLogical(expTokens), new syntax::IntExp(tgtLineno));
                statement = new statement::IfThenStatement(lineno, srcCode, new syntax::SyntaxTree(exp));
                break;
            }
            default:
                break;
        }
        return statement;
    }
}
