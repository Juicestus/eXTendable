#include "numbers.h"

void builtinNumbersIntegerParseInt(Var* c, void*) {
    std::string str = c->getParameter("str")->getString();
    int val = strtol(str.c_str(), 0, 0);
    c->getReturnVar()->setInt(val);
}

void builtinNumbersIntegerValueOf(Var* c, void*) {
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
    xt->addNative("function Numbers.parseInt(str)", builtinNumbersIntegerParseInt, 0);
    xt->addNative("function Numbers.valueOf(str)", builtinNumbersIntegerValueOf, 0);
    xt->addNative("function Numbers.charToInt(ch)", builtinNumbersCharToInt, 0);
}