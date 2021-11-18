#include "var.h"

Link::Link(Var *var, const std::string &name) {
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    this->name = name;
    this->nextSibling = 0;
    this->prevSibling = 0;
    this->var = var->ref();
    this->owned = false;
}

Link::Link(const Link &link) {
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

void Link::replaceWith(Var *newVar) {
    Var *oldVar = var;
    var = newVar->ref();
    oldVar->unref();
}

void Link::replaceWith(Link *newVar) {
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