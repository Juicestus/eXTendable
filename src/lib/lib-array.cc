#include "lib-array.h"

void builtinArrayContains(Var* c, void* data) {
    Var* obj = c->getParameter("obj");
    Link* v = c->getParameter("this")->firstChild;

    bool contains = false;
    while (v) {
        if (v->var->equals(obj)) {
            contains = true;
            break;
        }
        v = v->nextSibling;
    }

    c->getReturnVar()->setInt(contains);
}

void builtinArrayRemove(Var* c, void* data) {
    Var* obj = c->getParameter("obj");
    std::vector<int> removedIndices;
    Link* v;
    // remove
    v = c->getParameter("this")->firstChild;
    while (v) {
        if (v->var->equals(obj)) { removedIndices.push_back(v->getIntName()); }
        v = v->nextSibling;
    }
    // renumber
    v = c->getParameter("this")->firstChild;
    while (v) {
        int n = v->getIntName();
        int newn = n;
        for (size_t i = 0; i < removedIndices.size(); i++)
            if (n >= removedIndices[i]) newn--;
        if (newn != n) v->setIntName(newn);
        v = v->nextSibling;
    }
}

void builtinArrayJoin(Var* c, void* data) {
    std::string sep = c->getParameter("separator")->getString();
    Var* arr = c->getParameter("this");

    std::ostringstream sstr;
    int l = arr->getArrayLength();
    for (int i = 0; i < l; i++) {
        if (i > 0) sstr << sep;
        sstr << arr->getArrayIndex(i)->getString();
    }

    c->getReturnVar()->setString(sstr.str());
}

void loadArrayLibrary(XT* xt) {
    xt->addNative("function Array.contains(obj)", builtinArrayContains, 0);
    xt->addNative("function Array.remove(obj)", builtinArrayRemove, 0);
    xt->addNative("function Array.join(separator)", builtinArrayJoin, 0);
}