#ifndef XT_LEXER
#define XT_LEXER

#include "main.h"

#include "exception.h"
#include "token.h"
#include "utils.h"

class Lexer {
  public:
    Lexer(const std::string& input);
    Lexer(Lexer* owner, int startChar, int endChar);
    //~Lexer(void);
    ~Lexer();

    char currCh, nextCh;
    int tk, tokenStart, tokenEnd, tokenLastEnd;
    std::string tkStr;

    void match(int expectedTk);
    static std::string getTokenStr(int token);
    void reset();

    std::string getSubString(int pos);
    Lexer* getSubLex(int lastPosition);

    std::string getPosition(int pos = -1);

  protected:
    char* data;
    int dataStart, dataEnd;
    bool dataOwned;

    int dataPos;

    void getNextCh();
    void getNextToken();
};

#endif