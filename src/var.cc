#include "var.h"

Link::Link(Var* var, const std::string& name) {
#if DEBUG_MEMORY
    mark_allocated(this);
#endif
    this->constant = false;
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
    this->constant = false;
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
    if (constant && !var->isUndefined()) return;
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

void Link::makeConst() {
    this->constant = true;
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

Link* Var::addChildNoDup(const std::string& childName, Var* child) {
    if (!child) child = new Var();

    Link* v = findChild(childName);
    if (v)
        v->replaceWith(child);
    else
        v = addChild(childName, child);
    return v;
}

void Var::removeChild(Var* child) {
    Link* link = firstChild;
    while (link) {
        if (link->var == child) break;
        link = link->nextSibling;
    }
    ASSERT(link);
    removeLink(link);
}

void Var::removeLink(Link* link) {
    if (!link) return;
    if (link->nextSibling) link->nextSibling->prevSibling = link->prevSibling;
    if (link->prevSibling) link->prevSibling->nextSibling = link->nextSibling;
    if (lastChild == link) lastChild = link->prevSibling;
    if (firstChild == link) firstChild = link->nextSibling;
    delete link;
}

void Var::removeAllChildren() {
    Link* c = firstChild;
    while (c) {
        Link* t = c->nextSibling;
        delete c;
        c = t;
    }
    firstChild = 0;
    lastChild = 0;
}

Var* Var::getArrayIndex(int idx) {
    char sIdx[64];
    sprintf_s(sIdx, sizeof(sIdx), "%d", idx);
    Link* link = findChild(sIdx);
    if (link)
        return link->var;
    else
        return new Var(XT_BLANK_DATA, VAR_NULL); // undefined
}

void Var::setArrayIndex(int idx, Var* value) {
    char sIdx[64];
    sprintf_s(sIdx, sizeof(sIdx), "%d", idx);
    Link* link = findChild(sIdx);

    if (link) {
        if (value->isUndefined())
            removeLink(link);
        else
            link->replaceWith(value);
    } else {
        if (!value->isUndefined()) addChild(sIdx, value);
    }
}

int Var::getArrayLength() {
    int highest = -1;
    if (!isArray()) return 0;

    Link* link = firstChild;
    while (link) {
        if (isNumber(link->name)) {
            int val = atoi(link->name.c_str());
            if (val > highest) highest = val;
        }
        link = link->nextSibling;
    }
    return highest + 1;
}

int Var::getChildren() {
    int n = 0;
    Link* link = firstChild;
    while (link) {
        n++;
        link = link->nextSibling;
    }
    return n;
}

int Var::getInt() {
    if (isInt()) return intData;
    if (isNull()) return 0;
    if (isUndefined()) return 0;
    if (isDouble()) return (int)doubleData;
    return 0;
}

bool Var::getBool() {
    return getInt() != 0;
}

double Var::getDouble() {
    if (isDouble()) return doubleData;
    if (isInt()) return intData;
    if (isNull()) return 0;
    if (isUndefined()) return 0;
    return 0; // todo: or NaN? or throw?
}

const std::string& Var::getString() {
    static std::string snull = "null";
    static std::string sundefined = "undefined";
    if (isInt()) {
        char buffer[32];
        sprintf_s(buffer, sizeof(buffer), "%ld", intData);
        data = buffer;
        return data;
    }
    if (isDouble()) {
        char buffer[32];
        sprintf_s(buffer, sizeof(buffer), "%f", doubleData);
        data = buffer;
        return data;
    }
    if (isNull()) return snull;
    if (isUndefined()) return sundefined;
    return data;
}

void Var::setInt(int val) {
    flags = (flags & ~VAR_VARTYPEMASK) | VAR_INTEGER;
    intData = val;
    doubleData = 0;
    data = XT_BLANK_DATA;
}

void Var::setDouble(double val) {
    flags = (flags & ~VAR_VARTYPEMASK) | VAR_DOUBLE;
    doubleData = val;
    intData = 0;
    data = XT_BLANK_DATA;
}

void Var::setString(const std::string& str) {
    flags = (flags & ~VAR_VARTYPEMASK) | VAR_STRING;
    data = str;
    intData = 0;
    doubleData = 0;
}

void Var::setUndefined() {
    flags = (flags & ~VAR_VARTYPEMASK) | VAR_UNDEFINED;
    data = XT_BLANK_DATA;
    intData = 0;
    doubleData = 0;
    removeAllChildren();
}

void Var::setArray() {
    flags = (flags & ~VAR_VARTYPEMASK) | VAR_ARRAY;
    data = XT_BLANK_DATA;
    intData = 0;
    doubleData = 0;
    removeAllChildren();
}

bool Var::equals(Var* v) {
    Var* resV = mathsOp(v, TOK_EQUAL);
    bool res = resV->getBool();
    delete resV;
    return res;
}

bool Var::isInt() {
    return (flags & VAR_INTEGER) != 0;
}
bool Var::isDouble() {
    return (flags & VAR_DOUBLE) != 0;
}
bool Var::isString() {
    return (flags & VAR_STRING) != 0;
}
bool Var::isNumeric() {
    return (flags & VAR_NUMERICMASK) != 0;
}
bool Var::isFunction() {
    return (flags & VAR_FUNCTION) != 0;
}
bool Var::isObject() {
    return (flags & VAR_OBJECT) != 0;
}
bool Var::isArray() {
    return (flags & VAR_ARRAY) != 0;
}
bool Var::isNative() {
    return (flags & VAR_NATIVE) != 0;
}
bool Var::isUndefined() {
    return (flags & VAR_VARTYPEMASK) == VAR_UNDEFINED;
}
bool Var::isNull() {
    return (flags & VAR_NULL) != 0;
}
bool Var::isBasic() {
    return firstChild == 0;
}

Var* Var::mathsOp(Var* b, int op) {
    Var* a = this;
    if (op == TOK_TYPEEQUAL || op == TOK_NTYPEEQUAL) {
        bool eql =
                ((a->flags & VAR_VARTYPEMASK) == (b->flags & VAR_VARTYPEMASK));
        if (eql) {
            Var* contents = a->mathsOp(b, TOK_EQUAL);
            if (!contents->getBool()) eql = false;
            if (!contents->refs) delete contents;
        }
        if (op == TOK_TYPEEQUAL)
            return new Var(eql);
        else
            return new Var(!eql);
    }
    if (a->isUndefined() && b->isUndefined()) {
        if (op == TOK_EQUAL)
            return new Var(true);
        else if (op == TOK_NEQUAL)
            return new Var(false);
        else
            return new Var(); // undefined
    } else if ((a->isNumeric() || a->isUndefined()) &&
               (b->isNumeric() || b->isUndefined())) {
        if (!a->isDouble() && !b->isDouble()) {
            int da = a->getInt();
            int db = b->getInt();
            switch (op) {
            case '+': return new Var(da + db);
            case '-': return new Var(da - db);
            case '*': return new Var(da * db);
            case '/': return new Var(da / db);
            case '&': return new Var(da & db);
            case '|': return new Var(da | db);
            case '^': return new Var(da ^ db);
            case '%': return new Var(da % db);
            case TOK_EQUAL: return new Var(da == db);
            case TOK_NEQUAL: return new Var(da != db);
            case '<': return new Var(da < db);
            case TOK_LEQUAL: return new Var(da <= db);
            case '>': return new Var(da > db);
            case TOK_GEQUAL: return new Var(da >= db);
            default:
                throw new Exception("Operation " + Lexer::getTokenStr(op) +
                                    " not supported on the Int datatype");
            }
        } else {
            double da = a->getDouble();
            double db = b->getDouble();
            switch (op) {
            case '+': return new Var(da + db);
            case '-': return new Var(da - db);
            case '*': return new Var(da * db);
            case '/': return new Var(da / db);
            case TOK_EQUAL: return new Var(da == db);
            case TOK_NEQUAL: return new Var(da != db);
            case '<': return new Var(da < db);
            case TOK_LEQUAL: return new Var(da <= db);
            case '>': return new Var(da > db);
            case TOK_GEQUAL: return new Var(da >= db);
            default:
                throw new Exception("Operation " + Lexer::getTokenStr(op) +
                                    " not supported on the Double datatype");
            }
        }
    } else if (a->isArray()) {
        switch (op) {
        case TOK_EQUAL: return new Var(a == b);
        case TOK_NEQUAL: return new Var(a != b);
        default:
            throw new Exception("Operation " + Lexer::getTokenStr(op) +
                                " not supported on the Array datatype");
        }
    } else if (a->isObject()) {
        switch (op) {
        case TOK_EQUAL: return new Var(a == b);
        case TOK_NEQUAL: return new Var(a != b);
        default:
            throw new Exception("Operation " + Lexer::getTokenStr(op) +
                                " not supported on the Object datatype");
        }
    } else {
        std::string da = a->getString();
        std::string db = b->getString();
        switch (op) {
        case '+': return new Var(da + db, VAR_STRING);
        case TOK_EQUAL: return new Var(da == db);
        case TOK_NEQUAL: return new Var(da != db);
        case '<': return new Var(da < db);
        case TOK_LEQUAL: return new Var(da <= db);
        case '>': return new Var(da > db);
        case TOK_GEQUAL: return new Var(da >= db);
        default:
            throw new Exception("Operation " + Lexer::getTokenStr(op) +
                                " not supported on the string datatype");
        }
    }
    ASSERT(0);
    return 0;
}

void Var::copySimpleData(Var* val) {
    data = val->data;
    intData = val->intData;
    doubleData = val->doubleData;
    flags = (flags & ~VAR_VARTYPEMASK) | (val->flags & VAR_VARTYPEMASK);
}

void Var::copyValue(Var* val) {
    if (val) {
        copySimpleData(val);
        removeAllChildren();
        Link* child = val->firstChild;
        while (child) {
            Var* copied;
            if (child->name != XT_PROTOTYPE_CLASS)
                copied = child->var->deepCopy();
            else
                copied = child->var;

            addChild(child->name, copied);

            child = child->nextSibling;
        }
    } else {
        setUndefined();
    }
}

Var* Var::deepCopy() {
    Var* newVar = new Var();
    newVar->copySimpleData(this);
    Link* child = firstChild;
    while (child) {
        Var* copied;
        if (child->name != XT_PROTOTYPE_CLASS)
            copied = child->var->deepCopy();
        else
            copied = child->var;

        newVar->addChild(child->name, copied);
        child = child->nextSibling;
    }
    return newVar;
}

void Var::trace(std::string indentStr, const std::string& name) {
    TRACE("%s'%s' = '%s' %s\n", indentStr.c_str(), name.c_str(),
          getString().c_str(), getFlagsAsString().c_str());
    std::string indent = indentStr + " ";
    Link* link = firstChild;
    while (link) {
        link->var->trace(indent, link->name);
        link = link->nextSibling;
    }
}

std::string Var::getFlagsAsString() {
    std::string flagstr = "";
    if (flags & VAR_FUNCTION) flagstr = flagstr + "FUNCTION ";
    if (flags & VAR_OBJECT) flagstr = flagstr + "OBJECT ";
    if (flags & VAR_ARRAY) flagstr = flagstr + "ARRAY ";
    if (flags & VAR_NATIVE) flagstr = flagstr + "NATIVE ";
    if (flags & VAR_DOUBLE) flagstr = flagstr + "DOUBLE ";
    if (flags & VAR_INTEGER) flagstr = flagstr + "INTEGER ";
    if (flags & VAR_STRING) flagstr = flagstr + "STRING ";
    return flagstr;
}

std::string Var::getParsableString() {
    if (isNumeric()) return getString();
    if (isFunction()) {
        std::ostringstream funcStr;
        funcStr << "function (";
        Link* link = firstChild;
        while (link) {
            funcStr << link->name;
            if (link->nextSibling) funcStr << ",";
            link = link->nextSibling;
        }
        funcStr << ") " << getString();
        return funcStr.str();
    }
    if (isString()) return getXTString(getString());
    if (isNull()) return "null";
    return "undefined";
}

void Var::getJSON(std::ostringstream& destination,
                  const std::string linePrefix) {
    if (isObject()) {
        std::string indentedLinePrefix = linePrefix + "  ";
        destination << "{ \n";
        Link* link = firstChild;
        while (link) {
            destination << indentedLinePrefix;
            destination << getXTString(link->name);
            destination << " : ";
            link->var->getJSON(destination, indentedLinePrefix);
            link = link->nextSibling;
            if (link) { destination << ",\n"; }
        }
        destination << "\n" << linePrefix << "}";
    } else if (isArray()) {
        std::string indentedLinePrefix = linePrefix + "  ";
        destination << "[\n";
        int len = getArrayLength();
        if (len > 10000) len = 10000;

        for (int i = 0; i < len; i++) {
            getArrayIndex(i)->getJSON(destination, indentedLinePrefix);
            if (i < len - 1) destination << ",\n";
        }

        destination << "\n" << linePrefix << "]";
    } else {
        destination << getParsableString();
    }
}

void Var::setCallback(Callback callback, void* userdata) {
    this->callback = callback;
    this->callbackUserData = userdata;
}

Var* Var::ref() {
    refs++;
    return this;
}

void Var::unref() {
    if (refs <= 0) printf("OMFG, we have unreffed too far!\n");
    if ((--refs) == 0) { delete this; }
}

int Var::getRefs() {
    return refs;
}