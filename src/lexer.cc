#include "lexer.h"

Lexer::Lexer(const std::string &input) {
    data = strdup(input.c_str());
    dataOwned = true;
    dataStart = 0;
    dataEnd = strlen(data);
    reset();
}

Lexer::Lexer(Lexer *owner, int startChar, int endChar) {
    data = owner->data;
    dataOwned = false;
    dataStart = startChar;
    dataEnd = endChar;
    reset();
}

Lexer::~Lexer(void) {
    if (dataOwned)
        free((void*) data);
}

void Lexer::reset() {
    dataPos = dataStart;
    tokenStart = 0;
    tokenEnd = 0;
    tokenLastEnd = 0;
    tk = 0;
    tkStr = "";
    getNextCh();
    getNextCh();
    getNextToken();
}

void Lexer::match(int expectedTk) {
    if (tk != expectedTk)
        throw new Exception("Got " + getTokenStr(tk) + " expected " 
                + getTokenStr(expectedTk) + " at " + getPosition(tokenStart));
    getNextToken();
}




