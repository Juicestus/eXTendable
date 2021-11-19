#ifndef XT_VAR
#define XT_VAR

#include "main.h"

#include "exception.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"

// Frees the given link if it isn't owned by anything else
#define CLEAN(x)                                                               \
    {                                                                          \
        Link* __v = x;                                                         \
        if (__v && !__v->owned) delete __v;                                    \
    }

// Create a link to point to var and free the old link, but
// this is clever - it tries to keep the old link if it's
// not owned to save allocations.

#define CREATE_LINK(LINK, VAR)                                                 \
    {                                                                          \
        if (!LINK || LINK->owned)                                              \
            LINK = new CScriptVarLink(VAR);                                    \
        else                                                                   \
            LINK->replaceWith(VAR);                                            \
    }

enum VAR_FLAGS {
    VAR_UNDEFINED = 0,
    VAR_FUNCTION = 1,
    VAR_OBJECT = 2,
    VAR_ARRAY = 4,
    VAR_DOUBLE = 8,
    VAR_INTEGER = 16,
    VAR_STRING = 32,
    VAR_NULL = 64,

    VAR_NATIVE = 128,
    VAR_NUMERICMASK = VAR_NULL | VAR_DOUBLE | VAR_INTEGER,
    VAR_VARTYPEMASK = VAR_DOUBLE | VAR_INTEGER | VAR_STRING | VAR_FUNCTION |
                      VAR_OBJECT | VAR_ARRAY | VAR_NULL,
};

class Var;

typedef void (*Callback)(Var* var, void* userdata);

class Link {
  public:
    std::string name;
    Link* nextSibling;
    Link* prevSibling;
    Var* var;
    bool owned;

    Link(Var* var, const std::string& name = "");
    Link(const Link& link);
    ~Link();
    void replaceWith(Var* newVar);
    void replaceWith(Link* newVar);
    int getIntName();
    void setIntName(int n);
};

class Var {
  public:
    Var();
    Var(const std::string& varData, int varFlags);
    Var(const std::string& str);
    Var(double varData);
    Var(int val);
    //~Var(void);
    ~Var();

    Var* getReturnVar();
    void setReturnVar(Var* var);
    Var* getParameter(const std::string& name);

    Link* findChild(const std::string& childName);
    Link* findChildOrCreate(const std::string& childName,
                            int varFlags = VAR_UNDEFINED);
    Link* findChildOrCreateByPath(const std::string& path);
    Link* addChild(const std::string& childName, Var* child = NULL);
    Link* addChildNoDup(const std::string& childName, Var* child = NULL);
    void removeChild(Var* child);
    void removeLink(Link* link);
    void removeAllChildren();
    Var* getArrayIndex(int idx);
    void setArrayIndex(int idx, Var* value);
    int getArrayLength();
    int getChildren();

    int getInt();
    bool getBool();
    //{ return getInt() != 0; }
    double getDouble();
    const std::string& getString();
    std::string getParsableString();
    void setInt(int num);
    void setDouble(double val);
    void setString(const std::string& str);
    void setUndefined();
    void setArray();
    bool equals(Var* v);

    bool isInt();
    bool isDouble();
    bool isString();
    bool isNumeric();
    bool isFunction();
    bool isObject();
    bool isArray();
    bool isNative();
    bool isUndefined();
    bool isNull();
    bool isBasic();

    Var* mathsOp(Var* b, int op);
    void copyValue(Var* val);
    Var* deepCopy();

    void trace(std::string indentStr = "", const std::string& name = "");
    std::string getFlagsAsString();
    void getJSON(std::ostringstream& destination,
                 const std::string linePrefix = "");
    void setCallback(Callback callback, void* userdata);

    Link* firstChild;
    Link* lastChild;

    Var* ref();
    void unref();
    int getRefs();

  protected:
    int refs;

    std::string data;
    long intData;
    double doubleData;
    int flags;
    Callback callback;
    void* callbackUserData;

    void init();

    /** Copy the basic data and flags from the variable given, with no
     * children. Should be used internally only - by copyValue and deepCopy */
    void copySimpleData(Var* val);

    friend class XT;
};

#endif
