#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <unordered_map>

namespace xt {

typedef std::string TokenType;

class Token {
public:
    Token() {}
    Token(TokenType type, std::string literal) 
    : type(type), literal(literal) {}
    Token(TokenType type, char ch) 
    : type(type), literal(std::string(1, ch) ) {}

    TokenType type;
    std::string literal;
};

const TokenType ILLEGAL   = "ILLEGAL";
const TokenType END       = "END";
    
const TokenType IDENT     = "IDENT";
const TokenType INT       = "INT";
const TokenType STRING    = "STRING";

const TokenType ASSIGN    = "=";
const TokenType PLUS      = "+";
const TokenType MINUS     = "-";
const TokenType BANG      = "!";
const TokenType ASTERISK  = "*";
const TokenType SLASH     = "/";
const TokenType PERCENT   = "%";

const TokenType LT        = "<";
const TokenType GT        = ">";
const TokenType LE        = "<=";
const TokenType GE        = ">=";

const TokenType EQ        = "==";
const TokenType NE        = "!=";

const TokenType REF       = "&";
const TokenType PIPE      = "|";
const TokenType AND       = "&&";
const TokenType OR        = "||";

const TokenType PERIOD    = ".";
const TokenType COLON     = ":";
const TokenType MEMBER    = "::";
const TokenType COMMA     = ",";
const TokenType SEMICOLON = ";";

const TokenType ARROW     = "->";

const TokenType LPAREN    = "(";
const TokenType RPAREN    = ")";
const TokenType LBRACE    = "{";
const TokenType RBRACE    = "}";
const TokenType LBRACKET  = "[";
const TokenType RBRACKET  = "]";

const TokenType FUNCTION  = "FUNCTION";
const TokenType VAR       = "VAR";
const TokenType TRUE      = "TRUE";
const TokenType FALSE     = "FALSE";
const TokenType IF        = "IF";
const TokenType ELSE      = "ELSE";
const TokenType RETURN    = "RETURN";
const TokenType WHILE     = "WHILE";

extern std::unordered_map<std::string, TokenType> keywords;

TokenType lookUpIdentifier(const std::string& identifier);
}

#endif