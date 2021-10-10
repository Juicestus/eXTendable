#ifndef OBJECT_H_
#define OBJECT_H_

#include <vector>
#include <string>

#include "ast.h"

namespace xt {

typedef std::string ObjType;

const ObjType INTEGER_OBJ = "INTEGER";
const ObjType BOOLEAN_OBJ = "BOOLEAN";
const ObjType STRING_OBJ = "STRING";
const ObjType NULL_OBJ = "NULL";
const ObjType RETURN_VALUE_OBJ = "RETURN_VALUE";
const ObjType ERROR_OBJ = "ERROR";
const ObjType FUNCTION_OBJ = "FUNCTION";
const ObjType ARRAY_OBJ = "ARRAY";
const ObjType BUILTIN_OBJ = "BUILTIN";

class Object {
public:
    Object() : next(nullptr), mark(0) {}
    virtual ~Object() {}
    virtual ObjType Type() = 0;
    virtual std::string Inspect() = 0;

    Object* next;
    bool mark;
};

class Integer : public Object {
public:
    Integer(int v) : Object(), value(v) {}
    ObjType Type() { return INTEGER_OBJ; }
    std::string Inspect() { return std::to_string(value); }

    int value;
}




}

#endif