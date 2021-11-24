#include "lib-console.h"

void builtinPrint(Var* v, void* userdata) {
    std::printf("%s", v->getParameter("text")->getString().c_str());
}

void builtinPrintln(Var* v, void* userdata) {
    std::printf("%s\n", v->getParameter("text")->getString().c_str());
}

void builtinReadln(Var* c, void*) {
    std::string str;
    if (std::getline(std::cin, str)) c->getReturnVar()->setString(str);
}

void builtinPromptln(Var* c, void*) {
    std::string out = c->getParameter("text")->getString();
    std::printf("%s", out.c_str());
    std::string in;
    if (std::getline(std::cin, in)) c->getReturnVar()->setString(in);
}

void loadConsoleLibrary(XT* xt) {
    xt->addNative("function Console.print(text)", builtinPrint, 0);
    xt->addNative("function Console.println(text)", builtinPrintln, 0);
    xt->addNative("function Console.readln()", builtinReadln, 0);
    xt->addNative("function Console.promptln(text)", builtinPromptln, 0);
}
