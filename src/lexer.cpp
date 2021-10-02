#include "lexer.h"

#define ISLETTER(c) (isalpha(c) || c == '_')
#define ISDIGIT(c) (isdigit(c))

namespace xt {

void Lexer::readChar() {
    if (readPosition >= input.size()) ch = 0;
    else ch = input[readPosition];
    position = readPosition;
    readPosition++;
}

char Lexer::peekChar() {
    if (readPosition >= input.size()) return 0;
    return input[readPosition];
}

std::string Lexer::readIdentifier() {
    int start = position;
    while (ISLETTER(ch)) readChar();
    return input.substr(start, position - start);
}

std::string Lexer::readNumber() {
    int start = position;
    while (ISDIGIT(ch)) readChar();
    return input.substr(start, position - start);
}

std::string Lexer::readString() {
    int start = position + 1;
    do readChar(); 
    while (ch != '"' && ch != 0);
    return input.substr(start, position - start);
}

void Lexer::skipWhiteSpace() {
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') readChar();
    if (ch == '/' && peekChar() == '/') {
        while (ch != '\n') readChar();
        skipWhiteSpace(); // sexy recursion
    }
}

void Lexer::New(std::string &input) {
    this->input = input;
    readPosition = 0;
    readChar();
}

Token Lexer::nextToken() {
    Token token;
    skipWhiteSpace();
    switch (ch) {

        case '=':
            if (peekChar() == '=') {
                token = Token(EQ, "==");
                readChar();
            } else token = Token(ASSIGN, ch);
            readChar();
            break;

        case '!':
            if (peekChar() == '=') {
                token = Token(NE, "!=");
                readChar();
            } else token = Token(BANG, ch);
            readChar();
            break;

        case '<':
            if (peekChar() == '=') {
                token = Token(LE, "<=");
                readChar();
                } else token = Token(LT, ch);
            readChar();
            break;

        case '>':
            if (peekChar() == '=') {
                token = Token(GE, ">=");
                readChar();
                } else token = Token(GT, ch);
            readChar();
            break;

        case '&':
            if (peekChar() == '&') {
                token = Token(AND, "&&");
                readChar();
                } else token = Token(REF, ch);
            readChar();
            break;

        case '|':
            if (peekChar() == '|') {
                token = Token(OR, "||");
                readChar();
                } else token = Token(PIPE, ch);
            readChar();
            break;

        case '.':
            token = Token(PERIOD, ch);
            readChar();
            break;

        case ':':
            if (peekChar() == ':') {
                token = Token(MEMBER, "::");
                readChar();
                } else token = Token(COLON, ch);
            readChar();
            break;

        case ',':
            token = Token(COMMA, ch);
            readChar();
            break;

        case ';':
            token = Token(SEMICOLON, ch);
            readChar();
            break;       

        case '+':
            token = Token(PLUS, ch);
            readChar();
            break;

        case '-':
            if (peekChar() == '>') {
                token = Token(ARROW, "->");
                readChar();
                } else token = Token(MINUS, ch);
            readChar();
            break;

        case '*':
            token = Token(ASTERISK, ch);
            readChar();
            break;

        case '/':
            token = Token(SLASH, ch);
            readChar();
            break;

        case '%':
            token = Token(PERCENT, ch);
            readChar();
            break;
       
        case '(':
            token = Token(LPAREN, ch);
            readChar();
            break;

        case ')':
            token = Token(RPAREN, ch);
            readChar();
            break;

        case '{':
            token = Token(LBRACE, ch);
            readChar();
            break;

        case '}':
            token = Token(RBRACE, ch);
            readChar();
            break;

        case '[':
            token = Token(LBRACKET, ch);
            readChar();
            break;

        case ']':
            token = Token(RBRACKET, ch);
            readChar();
            break;

        case '"':
            token = Token(STRING, readString());
            readChar();
            break;
        
        case 0:
            token = Token(END, "");
            readChar();
            break;

        default:
            if(ISLETTER(ch)) {
                token.literal = readIdentifier();
                token.type = lookUpIdentifier(token.literal);
            } else if(ISDIGIT(ch)) {
                token.type = INT;
                token.literal = readNumber();
            } else {
                token = Token(ILLEGAL, ch);
                readChar();
            }
            break;
        }
        return token;
    }
}