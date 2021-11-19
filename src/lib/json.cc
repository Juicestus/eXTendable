#include "json.h"

void builtinJSONStringify(Var* c, void*) {
    std::ostringstream result;
    c->getParameter("obj")->getJSON(result);
    c->getReturnVar()->setString(result.str());
}

void loadJSONLibrary(XT* xt) {
    xt->addNative("function JSON.stringify(obj, replacer)",
                  builtinJSONStringify, 0);
   // JSON.parse is left out as you can (unsafely!) use eval instead
}
