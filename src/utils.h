#ifndef XT_UTILS
#define XT_UTILS

#include <iostream>
#include <string>

#include <stdio.h>
#include <string.h>

bool isWhitespace(char ch);
bool isNumeric(char ch);
bool isNumber(const std::string &str);
bool isHexadecimal(char ch);
bool isAlpha(char ch);
bool isIdentString(const char *s);

void replace(std::string &str, char textFrom, const char *textTo);
std::string getXTString(const std::string &str);

#endif