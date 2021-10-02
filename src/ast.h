#ifndef AST_H_
#define AST_H_

#include <string>
#include <vector>
#include "token.h"

namespace xt {

/* Interfaces */

class Node {
public:
    virtual ~Node() {}
    ///*
    virtual std::string tokenLiteral() = 0;
    virtual std::string string() = 0;
    virtual std::string type() = 0;
    //*/
    /*
    virtual std::string tokenLiteral();
    virtual std::string string();
    virtual std::string type();
    */
};

class Statement : public Node {
public:
    ///*
    virtual void statementNode() = 0;
    virtual std::string tokenLiteral() = 0;
    virtual std::string string() = 0;
    virtual std::string type() = 0;
    //*/
    /*
    virtual void statementNode();
    virtual std::string tokenLiteral();
    virtual std::string string();
    virtual std::string type();
    */
};

class Expression : public Node {
public:
    ///*
    virtual std::string tokenLiteral() = 0;
    virtual std::string string() = 0;
    virtual std::string type() = 0;
    //*/
    /*
    virtual std::string tokenLiteral();
    virtual std::string string();
    virtual std::string type();
    */
};

class Program : public Node {
public:
    ~Program() {
        for (auto stmt : statements) delete stmt;
    }
    std::string tokenLiteral();
    std::string string();
    std::string type() { return "Program"; }

    std::vector<Statement*> statements;  
};

class BlockStatement;

/* Statements */

class Identifier : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string() { return value; }
    std::string type() { return "Identifier"; }
  
    xt::Token token;
    std::string value;
};

class IntegerLiteral : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string() { return std::to_string(value); }
    std::string type() { return "IntegerLiteral"; }
    
    xt::Token token;
    int value;
};

class BooleanLiteral : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string() { return value ? "true" : "false"; }
    std::string type() { return "BooleanLiteral"; }
    
    xt::Token token;
    bool value;
};

class StringLiteral : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string() { return value; }
    std::string type() { return "StringLiteral"; }

    xt::Token token;
    std::string value;
};

class FunctionLiteral : public Expression {
public:
    ~FunctionLiteral() {
        for (auto param : parameters) delete param;
    }

    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "FunctionLiteral"; }
    
    xt::Token token;
    std::vector<Identifier*> parameters;
    BlockStatement* body;
};

class ArrayLiteral : public Expression {
public:
    ~ArrayLiteral() {
        for (auto elem : elements) delete elem;
    }

    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "ArrayLiteral"; }
    
    xt::Token token;
    std::vector<Expression*> elements;
};

class CallExpression : public Expression {
public:
    ~CallExpression() {
        for (auto arg : arguments) delete arg;
        delete function;
    }

    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "CallExpression"; }
    
    xt::Token token;
    Expression* function;
    std::vector<Expression*> arguments;
};

class IndexExpression : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "IndexExpression"; }

    xt::Token token;
    Expression* array;
    Expression* index;
};

class PrefixExpression : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "PrefixExpression"; }
    
    xt::Token token;
    std::string op;
    Expression* right;
};

class InfixExpression : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "InfixExpression"; }
    
    xt::Token token;
    Expression* left;
    std::string op;
    Expression* right;
};

class IfExpression : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "IfExpression"; }
  
    xt::Token token;
    Expression* condition;
    BlockStatement* consequence;
    BlockStatement* alternative;
};

class WhileExpression : public Expression {
public:
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "WhileExpression"; }
    
    xt::Token token;
    Expression* condition;
    BlockStatement* consequence;
};

/* Statements */

class VarStatement : public Statement {
 public:
    ~VarStatement() {
        delete value;
    }

    void statementNode() { }
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "VarStatement"; }

    xt::Token token;
    Identifier name;
    Expression* value;
};

class RefStatement : public Statement {
public:
    ~RefStatement() {
        delete value;
    }
    void statementNode() { }
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "RefStatement"; }

    xt::Token token;
    Identifier name;
    Expression* value;
};

class ReturnStatement : public Statement {
public:
    ~ReturnStatement() {
        delete returnValue;
    }
    void statementNode() { }
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "ReturnStatement"; }

    xt::Token token;
    Expression* returnValue;
};

class ExpressionStatement : public Statement {
public:
    ~ExpressionStatement() {
        delete expression;
    }
    void statementNode() {}
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "ExpressionStatement"; }

    xt::Token token;
    Expression* expression;
};

class BlockStatement : public Statement {
 public:
    ~BlockStatement() {
        for (auto stmt : statements) delete stmt;
    }
    void statementNode() {}
    std::string tokenLiteral() { return token.literal; }
    std::string string();
    std::string type() { return "BlockStatement"; }

    xt::Token token;
    std::vector<Statement*> statements;
};

}

#endif