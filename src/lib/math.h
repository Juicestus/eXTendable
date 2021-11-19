#ifndef XT_LIB_MATH
#define XT_LIB_MATH

#include "../common.h"

#include "../exception.h"
#include "../lexer.h"
#include "../token.h"
#include "../utils.h"
#include "../var.h"
#include "../xt.h"

#define k_E exp(1.0)
#define k_PI 3.1415926535897932384626433832795

#define F_ABS(a) ((a) >= 0 ? (a) : (-(a)))
#define F_MIN(a, b) ((a) > (b) ? (b) : (a))
#define F_MAX(a, b) ((a) > (b) ? (a) : (b))
#define F_SGN(a) ((a) > 0 ? 1 : ((a) < 0 ? -1 : 0))
#define F_RNG(a, min, max) ((a) < (min) ? min : ((a) > (max) ? max : a))
#define F_ROUND(a) ((a) > 0 ? (int)((a) + 0.5) : (int)((a)-0.5))

// Var shortcut macro
#define builtinIsInt(a) (c->getParameter(a)->isInt())
#define builtinIsDouble(a) (c->getParameter(a)->isDouble())
#define builtinGetInt(a) (c->getParameter(a)->getInt())
#define builtinGetDouble(a) (c->getParameter(a)->getDouble())
#define builtinReturnInt(a) (c->getReturnVar()->setInt(a))
#define builtinReturnDouble(a) (c->getReturnVar()->setDouble(a))

void loadMathLibrary(XT* xt);

#endif