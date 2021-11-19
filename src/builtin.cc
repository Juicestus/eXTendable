#include "builtin.h"

void builtinTrace(Var* c, void* userdata) {
    XT* xt = (XT*)userdata;
    xt->root->trace();
}

void builtinObjectDump(Var* c, void*) {
    c->getParameter("this")->trace("> ");
}

void builtinObjectClone(Var* c, void*) {
    Var* obj = c->getParameter("this");
    c->getReturnVar()->copyValue(obj);
}

void builtinNumbersCharToInt(Var* c, void*) {
    std::string str = c->getParameter("ch")->getString();
    ;
    int val = 0;
    if (str.length() > 0) val = (int)str.c_str()[0];
    c->getReturnVar()->setInt(val);
}

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


void builtinJSONStringify(Var* c, void*) {
    std::ostringstream result;
    c->getParameter("obj")->getJSON(result);
    c->getReturnVar()->setString(result.str());
}

void builtinExec(Var* c, void* data) {
    XT* xt = (XT*)data;
    std::string str = c->getParameter("code")->getString();
    xt->execute(str);
}

void builtinEval(Var* c, void* data) {
    XT* xt = (XT*)data;
    std::string str = c->getParameter("code")->getString();
    c->setReturnVar(xt->evaluateComplex(str).var);
}



void configureBuiltins(XT* xt) {
    xt->addNative("function exec(code)", builtinExec, xt);
    xt->addNative("function eval(code)", builtinEval, xt);
    xt->addNative("function trace()", builtinTrace, xt);
    xt->addNative("function Object.dump()", builtinObjectDump, 0);
    xt->addNative("function Object.clone()", builtinObjectClone, 0);
    xt->addNative("function JSON.stringify(obj, replacer)",
                  builtinJSONStringify, 0);
   // JSON.parse is left out as you can (unsafely!) use eval instead
 
}