#include "lib-lang.h"

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

void loadLangLibrary(XT* xt) {
    xt->addNative("function Lang.exec(code)", builtinExec, xt);
    xt->addNative("function Lang.eval(code)", builtinEval, xt);
    xt->addNative("function Lang.trace()", builtinTrace, xt);
    xt->addNative("function Lang.Object.dump()", builtinObjectDump, 0);
    xt->addNative("function Lang.Object.clone()", builtinObjectClone, 0);
}