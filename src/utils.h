#ifndef XT_UTILS
#define XT_UTILS

#include "main.h"

bool isWhitespace(char ch);
bool isNumeric(char ch);
bool isNumber(const std::string& str);
bool isHexadecimal(char ch);
bool isAlpha(char ch);
bool isIdentString(const char* s);

void replace(std::string& str, char textFrom, const char* textTo);
std::string getXTString(const std::string& str);

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args);

template<typename... Args>
std::string stringFormat(const std::string& format, Args... args);

std::string readFile(const std::string path);

#endif