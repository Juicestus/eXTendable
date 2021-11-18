#ifndef MAIN_H
#define MAIN_H

#include "exception.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"

#ifdef __GNUC__
#define vsprintf_s vsnprintf
#define sprintf_s snprintf
#define _strdup strdup
#define _snprintf std::snprintf
#endif

#endif