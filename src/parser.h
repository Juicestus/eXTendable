#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <string>
#include <unordered_map>
#include "lexer.h"
#include "ast.h"

namespace xt {

class Parser;

typedef Expression* (Parser::*prefixParseFn)();
typedef Expression* (Parser::*infixParseFn)(Expression*);

enum Precedence {
    LOWEST, EQUALS, LESSGREATER, SUM, PRODUCT, PREFIX, CALL, INDEX
};

extern std::unordered_map<TokenType, Precedence> precedences;

class Parser {
public:
    void New(Lexer& l);
    std::vector<std::string> getErrors() { return errors; }
    Program* parseProgram();

private:
    void nextToken();
    bool expectPeek(const TokenType& type);
    Precedence peekPrecedence();
    Precedence curPrecedence();

    Statement* parseStatement();
    VarStatement* parseVarStatement();
    RefStatement* parseRefStatement();
    ReturnStatement* parseReturnStatement();
    ExpressionStatement* parseExpressionStatement();
    BlockStatement* parseBlockStatement();

    Expression* parseExpression(Precedence precedence);

    Expression* parseIdentifier();
    Expression* parseIntegerLiteral();
    Expression* parseStringLiteral();
    Expression* parsePrefixExpression();
    Expression* parseBoolean();

    std::vector<Identifier*> parseFunctionParameters();

    Expression* parseFunctionLiteral();
    Expression* parseArrayLiteral();
    Expression* parseGroupedExpression();
    Expression* parseIfExpression();
    Expression* parseWhileExpression();

    std::vector<Expression*> parseExpressionList();

    Expression* parseCallExpression(Expression* function);
    Expression* parseIndexExpression(Expression* array);
    Expression* parseInfixExpression(Expression* left);
    
    void noPrefixParseFnError(TokenType type);
    void peekError(const TokenType& type);

    Lexer l;

    Token curToken;
    Token peekToken;
    
    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    std::vector<std::string> errors;
};

}

#endif