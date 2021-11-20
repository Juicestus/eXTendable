#ifndef XT_LIB_LANG
#define XT_LIB_LANG

#include "../common.h"

#include "../exception.h"
#include "../lexer.h"
#include "../token.h"
#include "../utils.h"
#include "../var.h"
#include "../xt.h"

void builtinEval(Var* c, void* data);

class XT;

void loadLangLibrary(XT* xt);

#endif