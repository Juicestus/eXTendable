#include "lib-math.h"

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

void builtinMathRand(Var* c, void*) {
    c->getReturnVar()->setDouble((double)rand() / RAND_MAX);
}

void builtinMathRandInt(Var* c, void*) {
    int min = c->getParameter("min")->getInt();
    int max = c->getParameter("max")->getInt();
    int val = min + (int)(rand() % (1 + max - min));
    c->getReturnVar()->setInt(val);
}


void loadMathLibrary(XT* xt) {
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

    xt->addNative("function Math.rand()", builtinMathRand, 0);
    xt->addNative("function Math.randInt(min, max)", builtinMathRandInt, 0);
}