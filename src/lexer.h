#ifndef LEXER_H_
#define LEXER_H_

#include <string>

#include "token.h"

namespace xt {

class Lexer {
public:
    void New(std::string &input);
    Token nextToken();

private:
    void readChar();
    char peekChar();
    std::string readIdentifier();
    std::string readNumber();
    std::string readString();
    void skipWhiteSpace();

    std::string input;
    int position;
    int readPosition;
    char ch;
};

}

#endif