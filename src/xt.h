#ifndef XT_XT
#define XT_XT

#include "common.h"

#include "exception.h"
#include "lexer.h"
#include "lib.h"
#include "token.h"
#include "utils.h"
#include "var.h"

class XT {
  public:
    XT(const std::string& path);
    ~XT();

    void setPath(const std::string& path);

    std::string path;
    std::string directory;

    void execute(const std::string& code);
    Link evaluateComplex(const std::string& code);
    std::string evaluate(const std::string& code);

    /**
        \code
           void nativeRandInt(Var *c, void *userdata) { ... }
           xt->addNative("function randInt(min, max)", nativeRandInt, 0);
        \endcode

        or

        \code
           void nativeSubstring(Var *c, void *userdata) { ... }
           xt->addNative("function String.substring(lo, hi)", nativeSubstring,
       0); \endcode
    */
    void addNative(const std::string& funcDesc, Callback ptr, void* userdata);

    void loadLibrary(const std::string& name);

    Var* getScriptVariable(const std::string& path);
    const std::string* getVariable(const std::string& path);
    bool setVariable(const std::string& path, const std::string& varData);

    void trace();

    Var* root;

  private:
    Lexer* l;
    std::vector<Var*> scopes;

#ifdef TINYJS_CALL_STACK
    std::vector<std::string> call_stack;
#endif

    Var* stringClass;
    Var* objectClass;
    Var* arrayClass;

    Link* functionCall(bool& execute, Link* function, Var* parent);
    Link* factor(bool& execute);
    Link* unary(bool& execute);
    Link* term(bool& execute);
    Link* expression(bool& execute);
    Link* shift(bool& execute);
    Link* condition(bool& execute);
    Link* logic(bool& execute);
    Link* ternary(bool& execute);
    Link* base(bool& execute);
    void block(bool& execute);
    void statement(bool& execute);
    Link* parseFunctionDefinition();
    void parseFunctionArguments(Var* funcVar);

    Link* findInScopes(const std::string& childName);
    Link* findInParentClasses(Var* object, const std::string& name);
};

#endif