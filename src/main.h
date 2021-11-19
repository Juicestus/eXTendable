#ifndef MAIN_H
#define MAIN_H

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define ASSERT(X) assert(X)

#define XT_LOOP_MAX_ITERATIONS 1000000000

#ifdef _WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif
#endif

#ifndef TRACE
#define TRACE printf
#endif

#ifdef __GNUC__
#define vsprintf_s vsnprintf
#define sprintf_s snprintf
#define _strdup strdup
#define _snprintf std::snprintf
#endif

#define XT_RETURN_VAR "return"
#define XT_PROTOTYPE_CLASS "prototype"
#define XT_TEMP_NAME ""
#define XT_BLANK_DATA ""

#endif