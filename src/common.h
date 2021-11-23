#ifndef COMMON_H
#define COMMON_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ASSERT(X) assert(X)

#define XT_LOOP_MAX_ITERATIONS 1000000000

#ifdef _WIN32
#include <windows.h>
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif
#else
#include <filesystem>
#endif

#ifndef TRACE
#define TRACE printf
#endif

#undef  __STRICT_ANSI__

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
#define XT_IMPORT_FUNCTION "import"

#endif
