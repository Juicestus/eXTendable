#include "lib-json.h"

#include "lib-lang.h"

void builtinJSONStringify(Var* c, void*) {
    std::ostringstream result;
    c->getParameter("obj")->getJSON(result);
    c->getReturnVar()->setString(result.str());
}

void loadJSONLibrary(XT* xt) {
    xt->addNative("function JSON.stringify(obj, replacer)",
                  builtinJSONStringify, 0);
    xt->addNative("function JSON.parse(code)", builtinEval, 0);
}
