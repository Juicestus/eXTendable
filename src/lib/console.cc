#include "console.h"

void builtinPrint(Var* v, void* userdata) {
    std::printf("%s", v->getParameter("text")->getString().c_str());
}

void builtinPrintln(Var* v, void* userdata) {
    std::printf("%s\n", v->getParameter("text")->getString().c_str());
}

void loadConsoleLibrary(XT* xt) {
    xt->addNative("function Console.print(text)", builtinPrint, 0);
    xt->addNative("function Console.println(text)", builtinPrintln, 0);
}
