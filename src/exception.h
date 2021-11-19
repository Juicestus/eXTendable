#ifndef XT_EXCEPTION
#define XT_EXCEPTION

#include "main.h"

class Exception {
  public:
    std::string text;
    Exception(const std::string& exceptionText);
};

#endif