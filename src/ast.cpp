#include "ast.h"

namespace xt {

std::string Program::tokenLiteral() {
    if (statements.size()) return statements[0]->tokenLiteral();
    return "";
}

std::string Program::string() {
    std::string result;
    for (auto stmt : statements) result += stmt->string() + "\n";
    return result;
}

std::string VarStatement::string() { 
    std::string result = tokenLiteral() + " " + name.string() + " = ";
    if (value != nullptr) result += value->string();
    return result + ";";
}

std::string RefStatement::string() {
    std::string result = tokenLiteral() + name.string() + " = ";
    if (value != nullptr) result += value->string();
    return result + ";";
}

std::string ReturnStatement::string() {
    std::string result = tokenLiteral() + " ";
    if (returnValue != nullptr) result += returnValue->string();
    return result + ";";
}

    
std::string ExpressionStatement::string() {
    std::string result = "";
    if (expression != nullptr) result += expression->string();
    return result + ";";
}

std::string BlockStatement::string() {
    std::string result = "{ ";
    for (auto stmt : statements) result += stmt->string() + " ";
    return result + "}";
}    

std::string PrefixExpression::string() {
    return "(" + op + right->string() + ")";
}

std::string InfixExpression::string() {
    return "(" + left->string() + op + right->string() + ")";
}

std::string IfExpression::string() {
    std::string result = "if " + condition->string() + " " + consequence->string();
    if (alternative != nullptr) result += " else  " + alternative->string();
    return result;
}

std::string WhileExpression::string() {
    return "while " + condition->string() + " " + consequence->string();
}

std::string FunctionLiteral::string() {
    std::string result = tokenLiteral() + " (";
    for (auto param : parameters) result += param->string() + ", ";
    return result + ")" + body->string();
}

std::string ArrayLiteral::string() {
    std::string result = "[";
    for (auto elem : elements) result += elem->string() + ", ";
    return result + "]";
}

std::string CallExpression::string() {
    std::string result = function->string() + "(";
    for (auto arg : arguments) result += arg->string() + ", ";
    return result + ")";
}

std::string IndexExpression::string() {
    return "(" + array->string() + "[" + index->string() + "])";
}

}