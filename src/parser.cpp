#include "parser.h"

namespace xt {

std::unordered_map<TokenType, Precedence> precedences({
    {EQ,        EQUALS},
    {NE,        EQUALS},
    {LT,        LESSGREATER},
    {GT,        LESSGREATER},
    {LE,        LESSGREATER},
    {GE,        LESSGREATER},
    {PLUS,      SUM},
    {MINUS,     SUM},
    {SLASH,     PRODUCT},
    {ASTERISK,  PRODUCT},
    {PERCENT,   PRODUCT},
    {LPAREN,    CALL},
    {LBRACKET,  INDEX},
});

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::expectPeek(const TokenType& type) {
    if (peekToken.type == type) {
        nextToken();
        return true;
    } else {
        peekError(type);
        return false;
    }
}

Precedence Parser::peekPrecedence() {
    if (precedences.find(peekToken.type) != precedences.end()) 
        return precedences[peekToken.type];
    return LOWEST;
}

Precedence Parser::curPrecedence() {
    if (precedences.find(curToken.type) != precedences.end()) 
        return precedences[curToken.type];
    return LOWEST;
}

void Parser::peekError(const TokenType& type) {
    errors.push_back("Expected " + type + ", got " + peekToken.type);
}

Statement* Parser::parseStatement() {
    if (curToken.type == VAR) return parseVarStatement();
    else if (curToken.type == REF) return parseRefStatement();
    else if (curToken.type == RETURN) return parseReturnStatement();
    else return parseExpressionStatement();   
}

VarStatement* Parser::parseVarStatement() { 
    VarStatement* stmt = new VarStatement();
    stmt->token = curToken;

    if (!expectPeek(IDENT)) {
        delete stmt;
        return nullptr;
    }

    stmt->name.token = curToken;
    stmt->name.value = curToken.literal;
    
    if (!expectPeek(ASSIGN)) {
        delete stmt;
        return nullptr;
    }
    nextToken();
    
    stmt->value = parseExpression(LOWEST);

    if (peekToken.type == SEMICOLON) nextToken();
    return stmt;
}

RefStatement* Parser::parseRefStatement() {
    RefStatement* stmt = new RefStatement();
    stmt->token = curToken;

    if (!expectPeek(IDENT)) {
        delete stmt;
        return nullptr;
    }

    stmt->name.token = curToken;
    stmt->name.value = curToken.literal;
    
    if (!expectPeek(ASSIGN)) {
        delete stmt;
        return nullptr;
    }
    nextToken();
    
    stmt->value = parseExpression(LOWEST);

    if (peekToken.type == SEMICOLON) nextToken();
    return stmt;
}

ReturnStatement* Parser::parseReturnStatement() {
    ReturnStatement* stmt = new ReturnStatement();
    stmt->token = curToken;

    nextToken();

    stmt->returnValue = parseExpression(LOWEST);

    if (peekToken.type == SEMICOLON) nextToken();
    return stmt;
}
    
ExpressionStatement* Parser::parseExpressionStatement() {
    ExpressionStatement* stmt = new ExpressionStatement();
    stmt->token = curToken;
    stmt->expression = parseExpression(LOWEST);

    if (peekToken.type == SEMICOLON) nextToken();
    return stmt;
}

BlockStatement* Parser::parseBlockStatement() {
    BlockStatement* block = new BlockStatement();
    block->token = curToken;
    
    while (curToken.type != RBRACE && curToken.type != END) {
        Statement* stmt = parseStatement();
        if (stmt != nullptr) block->statements.push_back(stmt);
        nextToken();
    }
    return block;
}

Expression* Parser::parseExpression(Precedence precedence) {
    if (prefixParseFns.find(curToken.type) == prefixParseFns.end()) {
        noPrefixParseFnError(curToken.type);
        return nullptr;
    }

    PrefixParseFn prefix = prefixParseFns[curToken.type];
    Expression* leftExpr = (this->*prefix)(); 

    while (peekToken.type != SEMICOLON && precedence < peekPrecedence()) {
        if (infixParseFns.find(peekToken.type) == infixParseFns.end()) return leftExpr;
        InfixParseFn infix = infixParseFns[peekToken.type];
        nextToken();
        leftExpr = (this->*infix)(leftExpr);
    }
    return leftExpr;
}

std::vector<Expression*> Parser::parseExpressionList() { 
    std::vector<Expression*> arguments;

    nextToken();
    arguments.push_back(parseExpression(LOWEST));

    while (peekToken.type == COMMA) {
        nextToken();
        nextToken();
        arguments.push_back(parseExpression(LOWEST));
    }

    return arguments;
}

Expression* Parser::parseIdentifier() {
    Identifier* ident = new Identifier();
    ident->token = curToken;
    ident->value = curToken.literal;
    return ident;
}

Expression* Parser::parseBoolean() { 
    BooleanLiteral* ident = new BooleanLiteral();
    ident->token = curToken;
    ident->value = curToken.type == TRUE;
    return ident; 
}

Expression* Parser::parseIntegerLiteral() {
    IntegerLiteral* integer = new IntegerLiteral();
    integer->token = curToken;
    // TODO: Then add floating point capability
    try {
        integer->value = std::stoi(curToken.literal);
    } catch (const std::exception& e) {
        errors.push_back("Invalid integer literal: " + curToken.literal);
        return nullptr;
    } 
    return integer;
}

Expression* Parser::parseStringLiteral() {
    StringLiteral* string = new StringLiteral();
    string->token = curToken;
    string->value = curToken.literal;
    return string;
}

std::vector<Identifier*> Parser::parseFunctionParameters() {
    std::vector<Identifier*> parameters;

    if (peekToken.type == RPAREN) {
        nextToken();
        return parameters;
    }

    nextToken();

    Identifier* identifier = new Identifier();
    identifier->token = curToken;
    identifier->value = curToken.literal;
    parameters.push_back(identifier);

    while (peekToken.type == COMMA) {
        nextToken();
        nextToken();
        Identifier* identifier = new Identifier();
        identifier->token = curToken;
        identifier->value = curToken.literal;
        parameters.push_back(identifier);
    }

    if (!expectPeek(RPAREN)) return std::vector<Identifier*>();
    return parameters;
}

Expression* Parser::parseFunctionLiteral() {
    FunctionLiteral* fn = new FunctionLiteral();
    fn->token = curToken;

    if (!expectPeek(LPAREN)) {
        delete fn;
        return nullptr;
    }

    fn->parameters = parseFunctionParameters();

    if (!expectPeek(LBRACE)) {
        delete fn;
        return nullptr;
    }

    fn->body = parseBlockStatement();
    return fn;
}

Expression* Parser::parseArrayLiteral() {
    ArrayLiteral* array = new ArrayLiteral();
    array->token = curToken;

    if (peekToken.type == RBRACKET) {
        nextToken();
        return array;
    }

    array->elements = parseExpressionList();
    if (!expectPeek(RBRACKET)) array->elements = std::vector<Expression*>();
    return array;
}

Expression* Parser::parsePrefixExpression() {
    PrefixExpression* expr = new PrefixExpression();
    expr->token = curToken;
    expr->op = curToken.literal;
    nextToken();
    expr->right = parseExpression(PREFIX);
    return expr;
}

Expression* Parser::parseGroupedExpression() {
    nextToken();
    Expression* expr = parseExpression(LOWEST);
    if (!expectPeek(RPAREN)) {
        delete expr;
        return nullptr;
    }
    return expr;
}

Expression* Parser::parseIfExpression() {
    IfExpression* expr = new IfExpression();
    expr->token = curToken;

    if (!expectPeek(LPAREN)) {
        delete expr;
        return nullptr;
    }

    nextToken();
    expr->condition = parseExpression(LOWEST);

    if (!expectPeek(RPAREN)) {
        delete expr;
        return nullptr;
    }

    if (!expectPeek(LBRACE)) {
        delete expr;
        return nullptr;
    }

    expr->consequence = parseBlockStatement();

    if (peekToken.type == ELSE) {
        nextToken();
        if (!expectPeek(LBRACE)) {
            delete expr;
            return nullptr;
        }
        expr->alternative = parseBlockStatement();
    }

    return expr;
}

Expression* Parser::parseWhileExpression() {
    WhileExpression* expr = new WhileExpression();
    expr->token = curToken;

    if (!expectPeek(LPAREN)) {
        delete expr;
        return nullptr;
    }

    nextToken();
    expr->condition = parseExpression(LOWEST);
    
    if (!expectPeek(RPAREN)) {
        delete expr;
        return nullptr;
    }

    if (!expectPeek(LBRACE)) {
        delete expr;
        return nullptr;
    }

    expr->consequence = parseBlockStatement();

    return expr;
}

Expression* Parser::parseCallExpression(Expression* function) {
    CallExpression* expr = new CallExpression();
    expr->token = curToken;
    expr->function = function;
    if (peekToken.type == RPAREN) {
        nextToken();
        return expr;
    }

    expr->arguments = parseExpressionList();
    if (!expectPeek(RPAREN)) expr->arguments = std::vector<Expression*>();
    return expr;
}

Expression* Parser::parseIndexExpression(Expression* array) {
    IndexExpression* expr = new IndexExpression();
    expr->token = curToken;
    expr->array = array;
    nextToken();
    expr->index = parseExpression(LOWEST);
    if (!expectPeek(RBRACKET)) return nullptr;
    return expr;
}

Expression* Parser::parseInfixExpression(Expression* left) {
    InfixExpression* expr = new InfixExpression();
    expr->token = curToken;
    expr->op = curToken.literal;
    expr->left = left;

    Precedence precedence = curPrecedence();
    nextToken();
    expr->right = parseExpression(precedence);
    return expr;
}



void Parser::noPrefixParseFnError(TokenType type) {
    errors.push_back("No prefix parse function for " + type);
}

void Parser::New(Lexer& lexer) {
    this->lexer = lexer;

    prefixParseFns[IDENT]    = &Parser::parseIdentifier;  
    prefixParseFns[INT]      = &Parser::parseIntegerLiteral;
    prefixParseFns[STRING]   = &Parser::parseStringLiteral;
    prefixParseFns[BANG]     = &Parser::parsePrefixExpression;
    prefixParseFns[MINUS]    = &Parser::parsePrefixExpression;
    prefixParseFns[TRUE]     = &Parser::parseBoolean;
    prefixParseFns[FALSE]    = &Parser::parseBoolean;
    prefixParseFns[LPAREN]   = &Parser::parseGroupedExpression;
    prefixParseFns[IF]       = &Parser::parseIfExpression;
    prefixParseFns[WHILE]    = &Parser::parseWhileExpression;
    prefixParseFns[FUNCTION] = &Parser::parseFunctionLiteral;
    prefixParseFns[LBRACKET] = &Parser::parseArrayLiteral;

    infixParseFns[PLUS]      = &Parser::parseInfixExpression;
    infixParseFns[MINUS]     = &Parser::parseInfixExpression;
    infixParseFns[SLASH]     = &Parser::parseInfixExpression;
    infixParseFns[ASTERISK]  = &Parser::parseInfixExpression;
    infixParseFns[PERCENT]   = &Parser::parseInfixExpression;
    infixParseFns[EQ]        = &Parser::parseInfixExpression;
    infixParseFns[NE]        = &Parser::parseInfixExpression;
    infixParseFns[LT]        = &Parser::parseInfixExpression;
    infixParseFns[GT]        = &Parser::parseInfixExpression;
    infixParseFns[LE]        = &Parser::parseInfixExpression;
    infixParseFns[GE]        = &Parser::parseInfixExpression;
    infixParseFns[LPAREN]    = &Parser::parseCallExpression;
    infixParseFns[LBRACKET]  = &Parser::parseIndexExpression;
    
    nextToken();
    nextToken();
}

Program* Parser::parseProgram() {
    Program* program = new Program();
    
    while (curToken.type != END) {
        Statement* statement = parseStatement();
        if (statement != nullptr) program->statements.push_back(statement);
        nextToken();
    } 
    return program;
}

}