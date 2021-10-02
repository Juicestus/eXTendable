#include "parser.h"

namespace xt {

std::unordered_map<TokenType, Precedence> precedences({
    {EQ,        EQUALS},
    {NE,        EQUALS},
    {LT,        LESSGREATER},
    {GT,        LESSGREATER},
    {LE,        LESSGREATER},
    {GE,        LESSGREATER},
    {PLUS,      SUM},
    {MINUS,     SUM},
    {SLASH,     PRODUCT},
    {ASTERISK,  PRODUCT},
    {PERCENT,   PRODUCT},
    {LPAREN,    CALL},
    {LBRACKET,  INDEX},
});

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l.nextToken();
}

bool Parser::expectPeek(const TokenType& type) {
    if (peekToken.type == type) {
        nextToken();
        return true;
    } else {
        peekError(type);
        return false;
    }
}

Precedence Parser::peekPrecedence() {
    if (precedences.find(peekToken.type) != precedences.end()) 
        return precedences[peekToken.type];
    return LOWEST;
}

Precedence Parser::curPrecedence() {
    if (precedences.find(curToken.type) != precedences.end()) 
        return precedences[curToken.type];
    return LOWEST;
}

void Parser::peekError(const TokenType& type) {
    errors.push_back("Expected " + type + ", got " + peekToken.type);
}

Statement* Parser::parseStatement() {
    if (curToken.type == VAR) return parseVarStatement();
    else if (curToken.type == REF) return parseRefStatement();
    else if (curToken.type == RETURN) return parseReturnStatement();
    else return parseExpressionStatement();   
}

VarStatement* Parser::parseVarStatement() { 
    VarStatement* stmt = new VarStatement();
    stmt->token = curToken;

    if (!expectPeek(IDENT)) {
        delete stmt;
        return nullptr;
    }

    stmt->name.token = curToken;
    stmt->name.value = curToken.literal;
    
    if (!expectPeek(IDENT)) {
        delete stmt;
        return nullptr;
    }
    nextToken();
    
    stmt->value = parseExpression(LOWEST);

    if (peekToken.type == SEMICOLON) nextToken();
    return stmt;
}

}