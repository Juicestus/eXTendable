#include "string.h"

void builtinStringIndexOf(Var* c, void*) {
    std::string str = c->getParameter("this")->getString();
    std::string search = c->getParameter("search")->getString();
    size_t p = str.find(search);
    int val = (p == std::string::npos) ? -1 : p;
    c->getReturnVar()->setInt(val);
}

void builtinStringSubstring(Var* c, void*) {
    std::string str = c->getParameter("this")->getString();
    int lo = c->getParameter("lo")->getInt();
    int hi = c->getParameter("hi")->getInt();

    int l = hi - lo;
    if (l > 0 && lo >= 0 && lo + l <= (int)str.length())
        c->getReturnVar()->setString(str.substr(lo, l));
    else
        c->getReturnVar()->setString("");
}

void builtinStringCharAt(Var* c, void*) {
    std::string str = c->getParameter("this")->getString();
    int p = c->getParameter("pos")->getInt();
    if (p >= 0 && p < (int)str.length())
        c->getReturnVar()->setString(str.substr(p, 1));
    else
        c->getReturnVar()->setString("");
}

void builtinStringCharCodeAt(Var* c, void*) {
    std::string str = c->getParameter("this")->getString();
    int p = c->getParameter("pos")->getInt();
    if (p >= 0 && p < (int)str.length())
        c->getReturnVar()->setInt(str.at(p));
    else
        c->getReturnVar()->setInt(0);
}

void builtinStringSplit(Var* c, void*) {
    std::string str = c->getParameter("this")->getString();
    std::string sep = c->getParameter("separator")->getString();
    Var* result = c->getReturnVar();
    result->setArray();
    int length = 0;

    size_t pos = str.find(sep);
    while (pos != std::string::npos) {
        result->setArrayIndex(length++, new Var(str.substr(0, pos)));
        str = str.substr(pos + 1);
        pos = str.find(sep);
    }

    if (str.size() > 0) result->setArrayIndex(length++, new Var(str));
}

void builtinStringFromCharCode(Var* c, void*) {
    char str[2];
    str[0] = c->getParameter("char")->getInt();
    str[1] = 0;
    c->getReturnVar()->setString(str);
}

void loadStringLibrary(XT* xt) {
    xt->addNative("function String.indexOf(search)", builtinStringIndexOf, 0);
    xt->addNative("function String.substring(lo,hi)", builtinStringSubstring,
                  0);
    xt->addNative("function String.charAt(pos)", builtinStringCharAt, 0);
    xt->addNative("function String.charCodeAt(pos)", builtinStringCharCodeAt,
                  0);
    xt->addNative("function String.fromCharCode(char)",
                  builtinStringFromCharCode, 0);
    xt->addNative("function String.split(separator)", builtinStringSplit, 0);
}