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

void builtinMathRand(Var* c, void*) {
    c->getReturnVar()->setDouble((double)rand() / RAND_MAX);
}

void builtinMathRandInt(Var* c, void*) {
    int min = c->getParameter("min")->getInt();
    int max = c->getParameter("max")->getInt();
    int val = min + (int)(rand() % (1 + max - min));
    c->getReturnVar()->setInt(val);
}

void builtinCharToInt(Var* c, void*) {
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

void builtinIntegerParseInt(Var* c, void*) {
    std::string str = c->getParameter("str")->getString();
    int val = strtol(str.c_str(), 0, 0);
    c->getReturnVar()->setInt(val);
}

void builtinIntegerValueOf(Var* c, void*) {
    std::string str = c->getParameter("str")->getString();

    int val = 0;
    if (str.length() == 1) val = str[0];
    c->getReturnVar()->setInt(val);
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

#define k_E exp(1.0)
#define k_PI 3.1415926535897932384626433832795

#define F_ABS(a) ((a) >= 0 ? (a) : (-(a)))
#define F_MIN(a, b) ((a) > (b) ? (b) : (a))
#define F_MAX(a, b) ((a) > (b) ? (a) : (b))
#define F_SGN(a) ((a) > 0 ? 1 : ((a) < 0 ? -1 : 0))
#define F_RNG(a, min, max) ((a) < (min) ? min : ((a) > (max) ? max : a))
#define F_ROUND(a) ((a) > 0 ? (int)((a) + 0.5) : (int)((a)-0.5))

// Var shortcut macro
#define builtinIsInt(a) (c->getParameter(a)->isInt())
#define builtinIsDouble(a) (c->getParameter(a)->isDouble())
#define builtinGetInt(a) (c->getParameter(a)->getInt())
#define builtinGetDouble(a) (c->getParameter(a)->getDouble())
#define builtinReturnInt(a) (c->getReturnVar()->setInt(a))
#define builtinReturnDouble(a) (c->getReturnVar()->setDouble(a))

#ifdef _MSC_VER
namespace {
double asinh(const double& value) {
    double returned;

    if (value > 0)
        returned = log(value + sqrt(value * value + 1));
    else
        returned = -log(-value + sqrt(value * value + 1));

    return (returned);
}

double acosh(const double& value) {
    double returned;

    if (value > 0)
        returned = log(value + sqrt(value * value - 1));
    else
        returned = -log(-value + sqrt(value * value - 1));

    return (returned);
}
} // namespace
#endif

// Math.abs(x) - returns absolute of given value
void builtinMathAbs(Var* c, void* userdata) {
    if (builtinIsInt("a")) {
        builtinReturnInt(F_ABS(builtinGetInt("a")));
    } else if (builtinIsDouble("a")) {
        builtinReturnDouble(F_ABS(builtinGetDouble("a")));
    }
}

// Math.round(a) - returns nearest round of given value
void builtinMathRound(Var* c, void* userdata) {
    if (builtinIsInt("a")) {
        builtinReturnInt(F_ROUND(builtinGetInt("a")));
    } else if (builtinIsDouble("a")) {
        builtinReturnDouble(F_ROUND(builtinGetDouble("a")));
    }
}

// Math.min(a,b) - returns minimum of two given values
void builtinMathMin(Var* c, void* userdata) {
    if ((builtinIsInt("a")) && (builtinIsInt("b"))) {
        builtinReturnInt(F_MIN(builtinGetInt("a"), builtinGetInt("b")));
    } else {
        builtinReturnDouble(
                F_MIN(builtinGetDouble("a"), builtinGetDouble("b")));
    }
}

// Math.max(a,b) - returns maximum of two given values
void builtinMathMax(Var* c, void* userdata) {
    if ((builtinIsInt("a")) && (builtinIsInt("b"))) {
        builtinReturnInt(F_MAX(builtinGetInt("a"), builtinGetInt("b")));
    } else {
        builtinReturnDouble(
                F_MAX(builtinGetDouble("a"), builtinGetDouble("b")));
    }
}

// Math.range(x,a,b) - returns value limited between two given values
void builtinMathRange(Var* c, void* userdata) {
    if ((builtinIsInt("x"))) {
        builtinReturnInt(F_RNG(builtinGetInt("x"), builtinGetInt("a"),
                               builtinGetInt("b")));
    } else {
        builtinReturnDouble(F_RNG(builtinGetDouble("x"), builtinGetDouble("a"),
                                  builtinGetDouble("b")));
    }
}

// Math.sign(a) - returns sign of given value (-1==negative,0=zero,1=positive)
void builtinMathSign(Var* c, void* userdata) {
    if (builtinIsInt("a")) {
        builtinReturnInt(F_SGN(builtinGetInt("a")));
    } else if (builtinIsDouble("a")) {
        builtinReturnDouble(F_SGN(builtinGetDouble("a")));
    }
}

// Math.PI() - returns PI value
void builtinMathPI(Var* c, void* userdata) {
    builtinReturnDouble(k_PI);
}

// Math.toDegrees(a) - returns degree value of a given angle in radians
void builtinMathToDegrees(Var* c, void* userdata) {
    builtinReturnDouble((180.0 / k_PI) * (builtinGetDouble("a")));
}

// Math.toRadians(a) - returns radians value of a given angle in degrees
void builtinMathToRadians(Var* c, void* userdata) {
    builtinReturnDouble((k_PI / 180.0) * (builtinGetDouble("a")));
}

// Math.sin(a) - returns trig. sine of given angle in radians
void builtinMathSin(Var* c, void* userdata) {
    builtinReturnDouble(sin(builtinGetDouble("a")));
}

// Math.asin(a) - returns trig. arcsine of given angle in radians
void builtinMathASin(Var* c, void* userdata) {
    builtinReturnDouble(asin(builtinGetDouble("a")));
}

// Math.cos(a) - returns trig. cosine of given angle in radians
void builtinMathCos(Var* c, void* userdata) {
    builtinReturnDouble(cos(builtinGetDouble("a")));
}

// Math.acos(a) - returns trig. arccosine of given angle in radians
void builtinMathACos(Var* c, void* userdata) {
    builtinReturnDouble(acos(builtinGetDouble("a")));
}

// Math.tan(a) - returns trig. tangent of given angle in radians
void builtinMathTan(Var* c, void* userdata) {
    builtinReturnDouble(tan(builtinGetDouble("a")));
}

// Math.atan(a) - returns trig. arctangent of given angle in radians
void builtinMathATan(Var* c, void* userdata) {
    builtinReturnDouble(atan(builtinGetDouble("a")));
}

// Math.sinh(a) - returns trig. hyperbolic sine of given angle in radians
void builtinMathSinh(Var* c, void* userdata) {
    builtinReturnDouble(sinh(builtinGetDouble("a")));
}

// Math.asinh(a) - returns trig. hyperbolic arcsine of given angle in radians
void builtinMathASinh(Var* c, void* userdata) {
    builtinReturnDouble(asinh((long double)builtinGetDouble("a")));
}

// Math.cosh(a) - returns trig. hyperbolic cosine of given angle in radians
void builtinMathCosh(Var* c, void* userdata) {
    builtinReturnDouble(cosh(builtinGetDouble("a")));
}

// Math.acosh(a) - returns trig. hyperbolic arccosine of given angle in radians
void builtinMathACosh(Var* c, void* userdata) {
    builtinReturnDouble(acosh((long double)builtinGetDouble("a")));
}

// Math.tanh(a) - returns trig. hyperbolic tangent of given angle in radians
void builtinMathTanh(Var* c, void* userdata) {
    builtinReturnDouble(tanh(builtinGetDouble("a")));
}

// Math.atan(a) - returns trig. hyperbolic arctangent of given angle in radians
void builtinMathATanh(Var* c, void* userdata) {
    builtinReturnDouble(atan(builtinGetDouble("a")));
}

// Math.E() - returns E Neplero value
void builtinMathE(Var* c, void* userdata) {
    builtinReturnDouble(k_E);
}

// Math.log(a) - returns natural logaritm (base E) of given value
void builtinMathLog(Var* c, void* userdata) {
    builtinReturnDouble(log(builtinGetDouble("a")));
}

// Math.log10(a) - returns logaritm(base 10) of given value
void builtinMathLog10(Var* c, void* userdata) {
    builtinReturnDouble(log10(builtinGetDouble("a")));
}

// Math.exp(a) - returns e raised to the power of a given number
void builtinMathExp(Var* c, void* userdata) {
    builtinReturnDouble(exp(builtinGetDouble("a")));
}

// Math.pow(a,b) - returns the result of a number raised to a power (a)^(b)
void builtinMathPow(Var* c, void* userdata) {
    builtinReturnDouble(pow(builtinGetDouble("a"), builtinGetDouble("b")));
}

// Math.sqr(a) - returns square of given value
void builtinMathSqr(Var* c, void* userdata) {
    builtinReturnDouble((builtinGetDouble("a") * builtinGetDouble("a")));
}

// Math.sqrt(a) - returns square root of given value
void builtinMathSqrt(Var* c, void* userdata) {
    builtinReturnDouble(sqrtf(builtinGetDouble("a")));
}

void builtinPrint(Var* v, void* userdata) {
    std::printf("%s", v->getParameter("text")->getString().c_str());
}

void builtinPrintln(Var* v, void* userdata) {
    std::printf("%s\n", v->getParameter("text")->getString().c_str());
}

void configureBuiltins(XT* xt) {
    xt->addNative("function exec(code)", builtinExec, xt);
    xt->addNative("function eval(code)", builtinEval, xt);
    xt->addNative("function trace()", builtinTrace, xt);
    xt->addNative("function Object.dump()", builtinObjectDump, 0);
    xt->addNative("function Object.clone()", builtinObjectClone, 0);
    xt->addNative("function Math.rand()", builtinMathRand, 0);
    xt->addNative("function Math.randInt(min, max)", builtinMathRandInt, 0);
    xt->addNative("function charToInt(ch)", builtinCharToInt, 0);
    xt->addNative("function String.indexOf(search)", builtinStringIndexOf, 0);
    xt->addNative("function String.substring(lo,hi)", builtinStringSubstring,
                  0);
    xt->addNative("function String.charAt(pos)", builtinStringCharAt, 0);
    xt->addNative("function String.charCodeAt(pos)", builtinStringCharCodeAt,
                  0);
    xt->addNative("function String.fromCharCode(char)",
                  builtinStringFromCharCode, 0);
    xt->addNative("function String.split(separator)", builtinStringSplit, 0);
    xt->addNative("function Integer.parseInt(str)", builtinIntegerParseInt, 0);
    xt->addNative("function Integer.valueOf(str)", builtinIntegerValueOf, 0);
    xt->addNative("function JSON.stringify(obj, replacer)",
                  builtinJSONStringify, 0);
    // JSON.parse is left out as you can (unsafely!) use eval instead
    xt->addNative("function Array.contains(obj)", builtinArrayContains, 0);
    xt->addNative("function Array.remove(obj)", builtinArrayRemove, 0);
    xt->addNative("function Array.join(separator)", builtinArrayJoin, 0);

    xt->addNative("function Math.abs(a)", builtinMathAbs, 0);
    xt->addNative("function Math.round(a)", builtinMathRound, 0);
    xt->addNative("function Math.min(a,b)", builtinMathMin, 0);
    xt->addNative("function Math.max(a,b)", builtinMathMax, 0);
    xt->addNative("function Math.range(x,a,b)", builtinMathRange, 0);
    xt->addNative("function Math.sign(a)", builtinMathSign, 0);

    xt->addNative("function Math.PI()", builtinMathPI, 0);
    xt->addNative("function Math.toDegrees(a)", builtinMathToDegrees, 0);
    xt->addNative("function Math.toRadians(a)", builtinMathToRadians, 0);
    xt->addNative("function Math.sin(a)", builtinMathSin, 0);
    xt->addNative("function Math.asin(a)", builtinMathASin, 0);
    xt->addNative("function Math.cos(a)", builtinMathCos, 0);
    xt->addNative("function Math.acos(a)", builtinMathACos, 0);
    xt->addNative("function Math.tan(a)", builtinMathTan, 0);
    xt->addNative("function Math.atan(a)", builtinMathATan, 0);
    xt->addNative("function Math.sinh(a)", builtinMathSinh, 0);
    xt->addNative("function Math.asinh(a)", builtinMathASinh, 0);
    xt->addNative("function Math.cosh(a)", builtinMathCosh, 0);
    xt->addNative("function Math.acosh(a)", builtinMathACosh, 0);
    xt->addNative("function Math.tanh(a)", builtinMathTanh, 0);
    xt->addNative("function Math.atanh(a)", builtinMathATanh, 0);

    xt->addNative("function Math.E()", builtinMathE, 0);
    xt->addNative("function Math.log(a)", builtinMathLog, 0);
    xt->addNative("function Math.log10(a)", builtinMathLog10, 0);
    xt->addNative("function Math.exp(a)", builtinMathExp, 0);
    xt->addNative("function Math.pow(a,b)", builtinMathPow, 0);

    xt->addNative("function Math.sqr(a)", builtinMathSqr, 0);
    xt->addNative("function Math.sqrt(a)", builtinMathSqrt, 0);

    xt->addNative("function Console.print(text)", builtinPrint, 0);
    xt->addNative("function Console.println(text)", builtinPrintln, 0);
}