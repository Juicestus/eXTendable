#include <iostream>

#include "../../include/var.h"

extern "C" void builtinTest(Var* v, void* userdata) {
    std::printf("Package loaded!\n");
}