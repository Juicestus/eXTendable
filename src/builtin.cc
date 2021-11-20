#include "builtin.h"

void loadStandardLibrary(XT *xt) {
    loadArrayLibrary(xt);
    loadConsoleLibrary(xt);
    loadJSONLibrary(xt);
    loadLangLibrary(xt);
    loadMathLibrary(xt);
    loadNumbersLibrary(xt);
    loadStringLibrary(xt);
}