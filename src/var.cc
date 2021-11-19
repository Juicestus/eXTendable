#include "var.h"

Link::Link(Var* var, const std::string& name) {
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    this->name = name;
    this->nextSibling = 0;
    this->prevSibling = 0;
    this->var = var->ref();
    this->owned = false;
}

Link::Link(const Link& link) {
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    this->name = link.name;
    this->nextSibling = 0;
    this->prevSibling = 0;
    this->var = link.var->ref();
    this->owned = false;
}

Link::~Link() {
#if DEBUG_MEMORY
    mark_deallocated(this);
#endif
    var->unref();
}

void Link::replaceWith(Var* newVar) {
    Var* oldVar = var;
    var = newVar->ref();
    oldVar->unref();
}

void Link::replaceWith(Link* newVar) {
    if (newVar)
        replaceWith(newVar->var);
    else
        replaceWith(new Var());
}

int Link::getIntName() {
    return atoi(name.c_str());
}

void Link::setIntName(int n) {
    char sIdx[64];
    sprintf_s(sIdx, sizeof(sIdx), "%d", n);
    name = sIdx;
}

Var::Var() {
    refs = 0;
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    init();
    flags = VAR_UNDEFINED;
}

Var::Var(const std::string& str) {
    refs = 0;
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    init();
    flags = VAR_STRING;
    data = str;
}

Var::Var(const std::string& varData, int varFlags) {
    refs = 0;
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    init();
    flags = varFlags;
    if (varFlags & VAR_INTEGER) {
        intData = strtol(varData.c_str(), 0, 0);
    } else if (varFlags & VAR_DOUBLE) {
        doubleData = strtod(varData.c_str(), 0);
    } else
        data = varData;
}

Var::Var(double val) {
    refs = 0;
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    init();
    setDouble(val);
}

Var::Var(int val) {
    refs = 0;
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    init();
    setInt(val);
}

Var::~Var(void) {
#if DEBUG_MEMORY
    mark_deallocated(this);
#endif
    removeAllChildren();
}

void Var::init() {
    firstChild = 0;
    lastChild = 0;
    flags = 0;
    callback = 0;
    callbackUserData = 0;
    data = XT_BLANK_DATA;
    intData = 0;
    doubleData = 0;
}

Var* Var::getReturnVar() {
    return getParameter(XT_RETURN_VAR);
}

void Var::setReturnVar(Var* var) {
    findChildOrCreate(XT_RETURN_VAR)->replaceWith(var);
}

Var* Var::getParameter(const std::string& name) {
    return findChildOrCreate(name)->var;
}

Link* Var::findChild(const std::string& childName) {
    Link* v = firstChild;
    while (v) {
        if (v->name.compare(childName) == 0) return v;
        v = v->nextSibling;
    }
    return 0;
}

Link* Var::findChildOrCreate(const std::string& childName, int varFlags) {
    Link* l = findChild(childName);
    if (l) return l;

    return addChild(childName, new Var(XT_BLANK_DATA, varFlags));
}

Link* Var::findChildOrCreateByPath(const std::string& path) {
    size_t p = path.find('.');
    if (p == std::string::npos) return findChildOrCreate(path);

    return findChildOrCreate(path.substr(0, p), VAR_OBJECT)
            ->var->findChildOrCreateByPath(path.substr(p + 1));
}

Link* Var::addChild(const std::string& childName, Var* child) {
    if (isUndefined()) { flags = VAR_OBJECT; }
    // if no child supplied, create one
    if (!child) child = new Var();

    Link* link = new Link(child, childName);
    link->owned = true;
    if (lastChild) {
        lastChild->nextSibling = link;
        link->prevSibling = lastChild;
        lastChild = link;
    } else {
        firstChild = link;
        lastChild = link;
    }
    return link;
}