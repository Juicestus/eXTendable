#include "xt.h"

void importDecl(Var* c, void*) {}

XT::XT() {
    l = 0;
    root = (new Var(XT_BLANK_DATA, VAR_OBJECT))->ref();
    stringClass = (new Var(XT_BLANK_DATA, VAR_OBJECT))->ref();
    arrayClass = (new Var(XT_BLANK_DATA, VAR_OBJECT))->ref();
    objectClass = (new Var(XT_BLANK_DATA, VAR_OBJECT))->ref();
    root->addChild("String", stringClass);
    root->addChild("Array", arrayClass);
    root->addChild("Object", objectClass);

    addNative("function import(name)", importDecl, 0);
}

XT::~XT() {
    ASSERT(!l);
    scopes.clear();
    stringClass->unref();
    arrayClass->unref();
    objectClass->unref();
    root->unref();

#if DEBUG_MEMORY
    show_allocated();
#endif
}

void XT::trace() {
    root->trace();
}


void XT::execute(const std::string& code) {
    Lexer* oldLex = l;
    std::vector<Var*> oldScopes = scopes;
    l = new Lexer(code);
#ifdef TINYJS_CALL_STACK
    call_stack.clear();
#endif
    scopes.clear();
    scopes.push_back(root);
    try {
        bool execute = true;
        while (l->tk) statement(execute);
    } catch (Exception* e) {
        std::ostringstream msg;
        msg << "Error " << e->text;
#ifdef TINYJS_CALL_STACK
        for (int i = (int)call_stack.size() - 1; i >= 0; i--)
            msg << "\n" << i << ": " << call_stack.at(i);
#endif
        msg << " at " << l->getPosition();
        delete l;
        l = oldLex;

        throw new Exception(msg.str());
    }
    delete l;
    l = oldLex;
    scopes = oldScopes;
}

Link XT::evaluateComplex(const std::string& code) {
    Lexer* oldLex = l;
    std::vector<Var*> oldScopes = scopes;

    l = new Lexer(code);
#ifdef TINYJS_CALL_STACK
    call_stack.clear();
#endif
    scopes.clear();
    scopes.push_back(root);
    Link* v = 0;
    try {
        bool execute = true;
        do {
            CLEAN(v);
            v = base(execute);
            if (l->tk != TOK_EOF) l->match(';');
        } while (l->tk != TOK_EOF);
    } catch (Exception* e) {
        std::ostringstream msg;
        msg << "Error " << e->text;
#ifdef TINYJS_CALL_STACK
        for (int i = (int)call_stack.size() - 1; i >= 0; i--)
            msg << "\n" << i << ": " << call_stack.at(i);
#endif
        msg << " at " << l->getPosition();
        delete l;
        l = oldLex;

        throw new Exception(msg.str());
    }
    delete l;
    l = oldLex;
    scopes = oldScopes;

    if (v) {
        Link r = *v;
        CLEAN(v);
        return r;
    }
    return Link(new Var());
}

std::string XT::evaluate(const std::string& code) {
    return evaluateComplex(code).var->getString();
}

void XT::parseFunctionArguments(Var* funcVar) {
    l->match('(');
    while (l->tk != ')') {
        funcVar->addChildNoDup(l->tkStr);
        l->match(TOK_ID);
        if (l->tk != ')') l->match(',');
    }
    l->match(')');
}

void XT::addNative(const std::string& funcDesc, Callback ptr, void* userdata) {
    Lexer* oldLex = l;
    l = new Lexer(funcDesc);

    Var* base = root;

    l->match(TOK_R_FUNCTION);
    std::string funcName = l->tkStr;
    l->match(TOK_ID);
    /* Check for dots, we might want to do something like function
     * String.substring ... */
    while (l->tk == '.') {
        l->match('.');
        Link* link = base->findChild(funcName);
        // if it doesn't exist, make an object class
        if (!link)
            link = base->addChild(funcName, new Var(XT_BLANK_DATA, VAR_OBJECT));
        base = link->var;
        funcName = l->tkStr;
        l->match(TOK_ID);
    }

    Var* funcVar = new Var(XT_BLANK_DATA, VAR_FUNCTION | VAR_NATIVE);
    funcVar->setCallback(ptr, userdata);
    parseFunctionArguments(funcVar);
    delete l;
    l = oldLex;

    base->addChild(funcName, funcVar);
}

void XT::loadLibrary(std::string name) {
    if (name == "Array")
        loadArrayLibrary(this);
    else if (name == "Console")
        loadConsoleLibrary(this);
    else if (name == "JSON")
        loadJSONLibrary(this);
    else if (name == "Lang")
        loadLangLibrary(this);
    else if (name == "Math")
        loadMathLibrary(this);
    else if (name == "Numbers")
        loadNumbersLibrary(this);
    else if (name == "String")
        loadStringLibrary(this);
}

Link* XT::parseFunctionDefinition() {
    // actually parse a function...
    l->match(TOK_R_FUNCTION);
    std::string funcName = XT_TEMP_NAME;
    /* we can have functions without names */
    if (l->tk == TOK_ID) {
        funcName = l->tkStr;
        l->match(TOK_ID);
    }
    Link* funcVar = new Link(new Var(XT_BLANK_DATA, VAR_FUNCTION), funcName);
    parseFunctionArguments(funcVar->var);
    int funcBegin = l->tokenStart;
    bool noexecute = false;
    block(noexecute);
    funcVar->var->data = l->getSubString(funcBegin);
    return funcVar;
}

/** Handle a function call (assumes we've parsed the function name and we're
 * on the start bracket). 'parent' is the object that contains this method,
 * if there was one (otherwise it's just a normnal function).
 */
Link* XT::functionCall(bool& execute, Link* function, Var* parent) {
    if (execute) {
        if (!function->var->isFunction()) {
            std::string errorMsg = "Expecting '";
            errorMsg = errorMsg + function->name + "' to be a function";
            throw new Exception(errorMsg.c_str());
        }
        l->match('(');
        // create a new symbol table entry for execution of this function
        Var* functionRoot = new Var(XT_BLANK_DATA, VAR_FUNCTION);
        if (parent) functionRoot->addChildNoDup("this", parent);
        // grab in all parameters
        Link* v = function->var->firstChild;
        while (v) {
            Link* value = base(execute);
            if (execute) {
                if (value->var->isBasic()) {
                    // pass by value
                    functionRoot->addChild(v->name, value->var->deepCopy());
                } else {
                    // pass by reference
                    functionRoot->addChild(v->name, value->var);
                }
            }
            CLEAN(value);
            if (l->tk != ')') l->match(',');
            v = v->nextSibling;
        }
        l->match(')');
        // setup a return variable
        Link* returnVar = NULL;
        // execute function!
        // add the function's execute space to the symbol table so we can
        // recurse
        Link* returnVarLink = functionRoot->addChild(XT_RETURN_VAR);
        scopes.push_back(functionRoot);
#ifdef TINYJS_CALL_STACK
        call_stack.push_back(function->name + " from " + l->getPosition());
#endif

        if (function->var->isNative()) {
            ASSERT(function->var->callback);
            if (function->name == XT_IMPORT_FUNCTION) {
                std::string lib = functionRoot->getParameter("name")->getString();
                loadLibrary(lib);
            }

            function->var->callback(functionRoot,
                                    function->var->callbackUserData);
        } else {
            Exception* exception = 0;
            Lexer* oldLex = l;
            Lexer* newLex = new Lexer(function->var->getString());
            l = newLex;
            try {
                block(execute);
                // because return will probably have called this, and set
                // execute to false
                execute = true;
            } catch (Exception* e) { exception = e; }
            delete newLex;
            l = oldLex;

            if (exception) throw exception;
        }
        scopes.pop_back();
        /* get the real return var before we remove it from our function */
        returnVar = new Link(returnVarLink->var);
        functionRoot->removeLink(returnVarLink);
        delete functionRoot;
        if (returnVar)
            return returnVar;
        else
            return new Link(new Var());
    } else {
        // function, but not executing - just parse args and be done
        l->match('(');
        while (l->tk != ')') {
            Link* value = base(execute);
            CLEAN(value);
            if (l->tk != ')') l->match(',');
        }
        l->match(')');
        if (l->tk == '{') { // TODO: why is this here?
            block(execute);
        }
        /* function will be a blank scriptvarlink if we're not executing,
         * so just return it rather than an alloc/free */
        return function;
    }
}

Link* XT::factor(bool& execute) {
    if (l->tk == '(') {
        l->match('(');
        Link* a = base(execute);
        l->match(')');
        return a;
    }
    if (l->tk == TOK_R_TRUE) {
        l->match(TOK_R_TRUE);
        return new Link(new Var(1));
    }
    if (l->tk == TOK_R_FALSE) {
        l->match(TOK_R_FALSE);
        return new Link(new Var(0));
    }
    if (l->tk == TOK_R_NULL) {
        l->match(TOK_R_NULL);
        return new Link(new Var(XT_BLANK_DATA, VAR_NULL));
    }
    if (l->tk == TOK_R_UNDEFINED) {
        l->match(TOK_R_UNDEFINED);
        return new Link(new Var(XT_BLANK_DATA, VAR_UNDEFINED));
    }
    if (l->tk == TOK_ID) {
        Link* a = execute ? findInScopes(l->tkStr) : new Link(new Var());
        // printf("0x%08X for %s at %s\n", (unsigned int)a, l->tkStr.c_str(),
        // l->getPosition().c_str());
        /* The parent if we're executing a method call */
        Var* parent = 0;

        if (execute && !a) {
            /* Variable doesn't exist! JavaScript says we should create it
             * (we won't add it here. This is done in the assignment operator)*/
            a = new Link(new Var(), l->tkStr);
        }
        l->match(TOK_ID);
        while (l->tk == '(' || l->tk == '.' || l->tk == '[') {
            if (l->tk ==
                '(') { // ------------------------------------- Function Call
                a = functionCall(execute, a, parent);
            } else if (l->tk == '.') { // -------------------------------------
                                       // Record Access
                l->match('.');
                if (execute) {
                    const std::string& name = l->tkStr;
                    Link* child = a->var->findChild(name);
                    if (!child) child = findInParentClasses(a->var, name);
                    if (!child) {
                        /* if we haven't found this defined yet, use the
                           built-in 'length' properly */
                        if (a->var->isArray() && name == "length") {
                            int l = a->var->getArrayLength();
                            child = new Link(new Var(l));
                        } else if (a->var->isString() && name == "length") {
                            int l = a->var->getString().size();
                            child = new Link(new Var(l));
                        } else {
                            child = a->var->addChild(name);
                        }
                    }
                    parent = a->var;
                    a = child;
                }
                l->match(TOK_ID);
            } else if (l->tk == '[') { // -------------------------------------
                                       // Array Access
                l->match('[');
                Link* index = base(execute);
                l->match(']');
                if (execute) {
                    Link* child =
                            a->var->findChildOrCreate(index->var->getString());
                    parent = a->var;
                    a = child;
                }
                CLEAN(index);
            } else
                ASSERT(0);
        }
        return a;
    }
    if (l->tk == TOK_INT || l->tk == TOK_FLOAT) {
        Var* a = new Var(l->tkStr,
                         ((l->tk == TOK_INT) ? VAR_INTEGER : VAR_DOUBLE));
        l->match(l->tk);
        return new Link(a);
    }
    if (l->tk == TOK_STR) {
        Var* a = new Var(l->tkStr, VAR_STRING);
        l->match(TOK_STR);
        return new Link(a);
    }
    if (l->tk == '{') {
        Var* contents = new Var(XT_BLANK_DATA, VAR_OBJECT);
        /* JSON-style object definition */
        l->match('{');
        while (l->tk != '}') {
            std::string id = l->tkStr;
            // we only allow strings or IDs on the left hand side of an
            // initialisation
            if (l->tk == TOK_STR)
                l->match(TOK_STR);
            else
                l->match(TOK_ID);
            l->match(':');
            if (execute) {
                Link* a = base(execute);
                contents->addChild(id, a->var);
                CLEAN(a);
            }
            // no need to clean here, as it will definitely be used
            if (l->tk != '}') l->match(',');
        }

        l->match('}');
        return new Link(contents);
    }
    if (l->tk == '[') {
        Var* contents = new Var(XT_BLANK_DATA, VAR_ARRAY);
        /* JSON-style array */
        l->match('[');
        int idx = 0;
        while (l->tk != ']') {
            if (execute) {
                char idx_str[16]; // big enough for 2^32
                sprintf_s(idx_str, sizeof(idx_str), "%d", idx);

                Link* a = base(execute);
                contents->addChild(idx_str, a->var);
                CLEAN(a);
            }
            // no need to clean here, as it will definitely be used
            if (l->tk != ']') l->match(',');
            idx++;
        }
        l->match(']');
        return new Link(contents);
    }
    if (l->tk == TOK_R_FUNCTION) {
        Link* funcVar = parseFunctionDefinition();
        if (funcVar->name != XT_TEMP_NAME)
            TRACE("Functions not defined at statement-level are not meant to "
                  "have a name");
        return funcVar;
    }
    if (l->tk == TOK_R_NEW) {
        // new -> create a new object
        l->match(TOK_R_NEW);
        const std::string& className = l->tkStr;
        if (execute) {
            Link* objClassOrFunc = findInScopes(className);
            if (!objClassOrFunc) {
                TRACE("%s is not a valid class name", className.c_str());
                return new Link(new Var());
            }
            l->match(TOK_ID);
            Var* obj = new Var(XT_BLANK_DATA, VAR_OBJECT);
            Link* objLink = new Link(obj);
            if (objClassOrFunc->var->isFunction()) {
                CLEAN(functionCall(execute, objClassOrFunc, obj));
            } else {
                obj->addChild(XT_PROTOTYPE_CLASS, objClassOrFunc->var);
                if (l->tk == '(') {
                    l->match('(');
                    l->match(')');
                }
            }
            return objLink;
        } else {
            l->match(TOK_ID);
            if (l->tk == '(') {
                l->match('(');
                l->match(')');
            }
        }
    }
    // Nothing we can do here... just hope it's the end...
    l->match(TOK_EOF);
    return 0;
}

Link* XT::unary(bool& execute) {
    Link* a;
    if (l->tk == '!') {
        l->match('!'); // binary not
        a = factor(execute);
        if (execute) {
            Var zero(0);
            Var* res = a->var->mathsOp(&zero, TOK_EQUAL);
            CREATE_LINK(a, res);
        }
    } else
        a = factor(execute);
    return a;
}

Link* XT::term(bool& execute) {
    Link* a = unary(execute);
    while (l->tk == '*' || l->tk == '/' || l->tk == '%') {
        int op = l->tk;
        l->match(l->tk);
        Link* b = unary(execute);
        if (execute) {
            Var* res = a->var->mathsOp(b->var, op);
            CREATE_LINK(a, res);
        }
        CLEAN(b);
    }
    return a;
}

Link* XT::expression(bool& execute) {
    bool negate = false;
    if (l->tk == '-') {
        l->match('-');
        negate = true;
    }
    Link* a = term(execute);
    if (negate) {
        Var zero(0);
        Var* res = zero.mathsOp(a->var, '-');
        CREATE_LINK(a, res);
    }

    while (l->tk == '+' || l->tk == '-' || l->tk == TOK_PLUSPLUS ||
           l->tk == TOK_MINUSMINUS) {
        int op = l->tk;
        l->match(l->tk);
        if (op == TOK_PLUSPLUS || op == TOK_MINUSMINUS) {
            if (execute) {
                Var one(1);
                Var* res =
                        a->var->mathsOp(&one, op == TOK_PLUSPLUS ? '+' : '-');
                Link* oldValue = new Link(a->var);
                a->replaceWith(res);
                CLEAN(a);
                a = oldValue;
            }
        } else {
            Link* b = term(execute);
            if (execute) {
                Var* res = a->var->mathsOp(b->var, op);
                CREATE_LINK(a, res);
            }
            CLEAN(b);
        }
    }
    return a;
}

Link* XT::shift(bool& execute) {
    Link* a = expression(execute);
    if (l->tk == TOK_LSHIFT || l->tk == TOK_RSHIFT ||
        l->tk == TOK_RSHIFTUNSIGNED) {
        int op = l->tk;
        l->match(op);
        Link* b = base(execute);
        int shift = execute ? b->var->getInt() : 0;
        CLEAN(b);
        if (execute) {
            if (op == TOK_LSHIFT) a->var->setInt(a->var->getInt() << shift);
            if (op == TOK_RSHIFT) a->var->setInt(a->var->getInt() >> shift);
            if (op == TOK_RSHIFTUNSIGNED)
                a->var->setInt(((unsigned int)a->var->getInt()) >> shift);
        }
    }
    return a;
}

Link* XT::condition(bool& execute) {
    Link* a = shift(execute);
    Link* b;
    while (l->tk == TOK_EQUAL || l->tk == TOK_NEQUAL ||
           l->tk == TOK_TYPEEQUAL || l->tk == TOK_NTYPEEQUAL ||
           l->tk == TOK_LEQUAL || l->tk == TOK_GEQUAL || l->tk == '<' ||
           l->tk == '>') {
        int op = l->tk;
        l->match(l->tk);
        b = shift(execute);
        if (execute) {
            Var* res = a->var->mathsOp(b->var, op);
            CREATE_LINK(a, res);
        }
        CLEAN(b);
    }
    return a;
}

Link* XT::logic(bool& execute) {
    Link* a = condition(execute);
    Link* b;
    while (l->tk == '&' || l->tk == '|' || l->tk == '^' ||
           l->tk == TOK_ANDAND || l->tk == TOK_OROR) {
        bool noexecute = false;
        int op = l->tk;
        l->match(l->tk);
        bool shortCircuit = false;
        bool boolean = false;
        // if we have short-circuit ops, then if we know the outcome
        // we don't bother to execute the other op. Even if not
        // we need to tell mathsOp it's an & or |
        if (op == TOK_ANDAND) {
            op = '&';
            shortCircuit = !a->var->getBool();
            boolean = true;
        } else if (op == TOK_OROR) {
            op = '|';
            shortCircuit = a->var->getBool();
            boolean = true;
        }
        b = condition(shortCircuit ? noexecute : execute);
        if (execute && !shortCircuit) {
            if (boolean) {
                Var* newa = new Var(a->var->getBool());
                Var* newb = new Var(b->var->getBool());
                CREATE_LINK(a, newa);
                CREATE_LINK(b, newb);
            }
            Var* res = a->var->mathsOp(b->var, op);
            CREATE_LINK(a, res);
        }
        CLEAN(b);
    }
    return a;
}

Link* XT::ternary(bool& execute) {
    Link* lhs = logic(execute);
    bool noexec = false;
    if (l->tk == '?') {
        l->match('?');
        if (!execute) {
            CLEAN(lhs);
            CLEAN(base(noexec));
            l->match(':');
            CLEAN(base(noexec));
        } else {
            bool first = lhs->var->getBool();
            CLEAN(lhs);
            if (first) {
                lhs = base(execute);
                l->match(':');
                CLEAN(base(noexec));
            } else {
                CLEAN(base(noexec));
                l->match(':');
                lhs = base(execute);
            }
        }
    }

    return lhs;
}

Link* XT::base(bool& execute) {
    Link* lhs = ternary(execute);
    if (l->tk == '=' || l->tk == TOK_PLUSEQUAL || l->tk == TOK_MINUSEQUAL) {
        /* If we're assigning to this and we don't have a parent,
         * add it to the symbol table root as per JavaScript. */
        if (execute && !lhs->owned) {
            if (lhs->name.length() > 0) {
                Link* realLhs = root->addChildNoDup(lhs->name, lhs->var);
                CLEAN(lhs);
                lhs = realLhs;
            } else
                TRACE("Trying to assign to an un-named type\n");
        }

        int op = l->tk;
        l->match(l->tk);
        Link* rhs = base(execute);
        if (execute) {
            if (op == '=') {
                lhs->replaceWith(rhs);
            } else if (op == TOK_PLUSEQUAL) {
                Var* res = lhs->var->mathsOp(rhs->var, '+');
                lhs->replaceWith(res);
            } else if (op == TOK_MINUSEQUAL) {
                Var* res = lhs->var->mathsOp(rhs->var, '-');
                lhs->replaceWith(res);
            } else
                ASSERT(0);
        }
        CLEAN(rhs);
    }
    return lhs;
}

void XT::block(bool& execute) {
    l->match('{');
    if (execute) {
        while (l->tk && l->tk != '}') statement(execute);
        l->match('}');
    } else {
        // fast skip of blocks
        int brackets = 1;
        while (l->tk && brackets) {
            if (l->tk == '{') brackets++;
            if (l->tk == '}') brackets--;
            l->match(l->tk);
        }
    }
}

void XT::statement(bool& execute) {
    if (l->tk == TOK_ID || l->tk == TOK_INT || l->tk == TOK_FLOAT ||
        l->tk == TOK_STR || l->tk == '-') {
        /* Execute a simple statement that only contains basic arithmetic... */
        CLEAN(base(execute));
        l->match(';');
    } else if (l->tk == '{') {
        /* A block of code */
        block(execute);
    } else if (l->tk == ';') {
        /* Empty statement - to allow things like ;;; */
        l->match(';');
    } else if (l->tk == TOK_R_VAR) {
        /* variable creation. TODO - we need a better way of parsing the left
         * hand side. Maybe just have a flag called can_create_var that we
         * set and then we parse as if we're doing a normal equals.*/
        l->match(TOK_R_VAR);
        while (l->tk != ';') {
            Link* a = 0;
            if (execute) a = scopes.back()->findChildOrCreate(l->tkStr);
            l->match(TOK_ID);
            // now do stuff defined with dots
            while (l->tk == '.') {
                l->match('.');
                if (execute) {
                    Link* lastA = a;
                    a = lastA->var->findChildOrCreate(l->tkStr);
                }
                l->match(TOK_ID);
            }
            // sort out initialiser
            if (l->tk == '=') {
                l->match('=');
                Link* var = base(execute);
                if (execute) a->replaceWith(var);
                CLEAN(var);
            }
            if (l->tk != ';') l->match(',');
        }
        l->match(';');
    } else if (l->tk == TOK_R_IF) {
        l->match(TOK_R_IF);
        l->match('(');
        Link* var = base(execute);
        l->match(')');
        bool cond = execute && var->var->getBool();
        CLEAN(var);
        bool noexecute = false; // because we need to be abl;e to write to it
        statement(cond ? execute : noexecute);
        if (l->tk == TOK_R_ELSE) {
            l->match(TOK_R_ELSE);
            statement(cond ? noexecute : execute);
        }
    } else if (l->tk == TOK_R_WHILE) {
        // We do repetition by pulling out the string representing our statement
        // there's definitely some opportunity for optimisation here
        l->match(TOK_R_WHILE);
        l->match('(');
        int whileCondStart = l->tokenStart;
        bool noexecute = false;
        Link* cond = base(execute);
        bool loopCond = execute && cond->var->getBool();
        CLEAN(cond);
        Lexer* whileCond = l->getSubLex(whileCondStart);
        l->match(')');
        int whileBodyStart = l->tokenStart;
        statement(loopCond ? execute : noexecute);
        Lexer* whileBody = l->getSubLex(whileBodyStart);
        Lexer* oldLex = l;
        int loopCount = XT_LOOP_MAX_ITERATIONS;
        while (loopCond && loopCount-- > 0) {
            whileCond->reset();
            l = whileCond;
            cond = base(execute);
            loopCond = execute && cond->var->getBool();
            CLEAN(cond);
            if (loopCond) {
                whileBody->reset();
                l = whileBody;
                statement(execute);
            }
        }
        l = oldLex;
        delete whileCond;
        delete whileBody;

        if (loopCount <= 0) {
            root->trace();
            TRACE("WHILE Loop exceeded %d iterations at %s\n",
                  XT_LOOP_MAX_ITERATIONS, l->getPosition().c_str());
            throw new Exception("LOOP_ERROR");
        }
    } else if (l->tk == TOK_R_FOR) {
        l->match(TOK_R_FOR);
        l->match('(');
        statement(execute); // initialisation
        // l->match(';');
        int forCondStart = l->tokenStart;
        bool noexecute = false;
        Link* cond = base(execute); // condition
        bool loopCond = execute && cond->var->getBool();
        CLEAN(cond);
        Lexer* forCond = l->getSubLex(forCondStart);
        l->match(';');
        int forIterStart = l->tokenStart;
        CLEAN(base(noexecute)); // iterator
        Lexer* forIter = l->getSubLex(forIterStart);
        l->match(')');
        int forBodyStart = l->tokenStart;
        statement(loopCond ? execute : noexecute);
        Lexer* forBody = l->getSubLex(forBodyStart);
        Lexer* oldLex = l;
        if (loopCond) {
            forIter->reset();
            l = forIter;
            CLEAN(base(execute));
        }
        int loopCount = XT_LOOP_MAX_ITERATIONS;
        while (execute && loopCond && loopCount-- > 0) {
            forCond->reset();
            l = forCond;
            cond = base(execute);
            loopCond = cond->var->getBool();
            CLEAN(cond);
            if (execute && loopCond) {
                forBody->reset();
                l = forBody;
                statement(execute);
            }
            if (execute && loopCond) {
                forIter->reset();
                l = forIter;
                CLEAN(base(execute));
            }
        }
        l = oldLex;
        delete forCond;
        delete forIter;
        delete forBody;
        if (loopCount <= 0) {
            root->trace();
            TRACE("FOR Loop exceeded %d iterations at %s\n",
                  XT_LOOP_MAX_ITERATIONS, l->getPosition().c_str());
            throw new Exception("LOOP_ERROR");
        }
    } else if (l->tk == TOK_R_RETURN) {
        l->match(TOK_R_RETURN);
        Link* result = 0;
        if (l->tk != ';') result = base(execute);
        if (execute) {
            Link* resultVar = scopes.back()->findChild(XT_RETURN_VAR);
            if (resultVar)
                resultVar->replaceWith(result);
            else
                TRACE("RETURN statement, but not in a function.\n");
            execute = false;
        }
        CLEAN(result);
        l->match(';');
    } else if (l->tk == TOK_R_FUNCTION) {
        Link* funcVar = parseFunctionDefinition();
        if (execute) {
            if (funcVar->name == XT_TEMP_NAME)
                TRACE("Functions defined at statement-level are meant to have "
                      "a name\n");
            else
                scopes.back()->addChildNoDup(funcVar->name, funcVar->var);
        }
        CLEAN(funcVar);
    } else
        l->match(TOK_EOF);
}

Var* XT::getScriptVariable(const std::string& path) {
    size_t prevIdx = 0;
    size_t thisIdx = path.find('.');
    if (thisIdx == std::string::npos) thisIdx = path.length();
    Var* var = root;
    while (var && prevIdx < path.length()) {
        std::string el = path.substr(prevIdx, thisIdx - prevIdx);
        Link* varl = var->findChild(el);
        var = varl ? varl->var : 0;
        prevIdx = thisIdx + 1;
        thisIdx = path.find('.', prevIdx);
        if (thisIdx == std::string::npos) thisIdx = path.length();
    }
    return var;
}

const std::string* XT::getVariable(const std::string& path) {
    Var* var = getScriptVariable(path);
    if (var)
        return &var->getString();
    else
        return 0;
}

bool XT::setVariable(const std::string& path, const std::string& varData) {
    Var* var = getScriptVariable(path);
    if (var) {
        if (var->isInt())
            var->setInt((int)strtol(varData.c_str(), 0, 0));
        else if (var->isDouble())
            var->setDouble(strtod(varData.c_str(), 0));
        else
            var->setString(varData.c_str());
        return true;
    } else
        return false;
}

Link* XT::findInScopes(const std::string& childName) {
    for (int s = scopes.size() - 1; s >= 0; s--) {
        Link* v = scopes[s]->findChild(childName);
        if (v) return v;
    }
    return NULL;
}

Link* XT::findInParentClasses(Var* object, const std::string& name) {
    Link* parentClass = object->findChild(XT_PROTOTYPE_CLASS);
    while (parentClass) {
        Link* implementation = parentClass->var->findChild(name);
        if (implementation) return implementation;
        parentClass = parentClass->var->findChild(XT_PROTOTYPE_CLASS);
    }
    if (object->isString()) {
        Link* implementation = stringClass->findChild(name);
        if (implementation) return implementation;
    }
    if (object->isArray()) {
        Link* implementation = arrayClass->findChild(name);
        if (implementation) return implementation;
    }
    Link* implementation = objectClass->findChild(name);
    if (implementation) return implementation;

    return 0;
}
