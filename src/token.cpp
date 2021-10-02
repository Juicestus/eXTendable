#include "token.h"

namespace xt {

std::unordered_map<std::string, TokenType> keywords({
    {"var", VAR},
    {"function", FUNCTION},
    {"true", TRUE},
    {"false", FALSE},
    {"if", IF},
    {"else", ELSE},
    {"return", RETURN},
    {"while", WHILE}
});

TokenType lookUpIdentifier(const std::string& identifier) {
    if (keywords.find(identifier)== keywords.end())
        return IDENT;
    return keywords[identifier];
    }
}