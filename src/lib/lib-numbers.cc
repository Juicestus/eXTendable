#include "lib-numbers.h"

void builtinNumbersParseInt(Var* c, void*) {
    std::string str = c->getParameter("str")->getString();
    int val = strtol(str.c_str(), 0, 0);
    c->getReturnVar()->setInt(val);
}

void builtinNumbersParseDouble(Var* c, void*) {
    std::string str = c->getParameter("str")->getString();
    char *ptr;
    double val = strtod(str.c_str(), &ptr);
    free(ptr);
    c->getReturnVar()->setDouble(val);
}

void builtinNumbersValueOf(Var* c, void*) {
    std::string str = c->getParameter("str")->getString();

    int val = 0;
    if (str.length() == 1) val = str[0];
    c->getReturnVar()->setInt(val);
}

void builtinNumbersCharToInt(Var* c, void*) {
    std::string str = c->getParameter("ch")->getString();
    ;
    int val = 0;
    if (str.length() > 0) val = (int)str.c_str()[0];
    c->getReturnVar()->setInt(val);
}

void loadNumbersLibrary(XT* xt) {
    xt->addNative("function Numbers.parseInt(str)", builtinNumbersParseInt, 0);
    xt->addNative("function Numbers.parseDouble(str)", builtinNumbersParseDouble, 0);
    xt->addNative("function Numbers.valueOf(str)", builtinNumbersValueOf, 0);
    xt->addNative("function Numbers.charToInt(ch)", builtinNumbersCharToInt, 0);
}