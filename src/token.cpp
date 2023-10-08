#include "token.hpp"

Token::Token(Token::Type type, Operator op) {
    this->type = type;
    this->op = op;
}

Token::Token(Token::Type type, char* iden) {
    this->type = type;
    this->iden = iden;
}

Token::Token(Token::Type type, Separator sep) {
    this->type = type;
    this->sep = sep;
}

Token::Token(Token::Type type, int32_t intLit) {
    this->type = type;
    this->intLit = intLit;
}

Token::Token(Token::Type type, float floatLit) {
    this->type = type;
    this->floatLit = floatLit;
}

Token::Token(Token::Type type) {
    this->type = type;
}