#ifndef XT_EXCEPTION
#define XT_EXCEPTION

#include <iostream>
#include <string>

class Exception {
public:
    std::string text;
    Exception(const std::string &exceptionText);
};

#endif