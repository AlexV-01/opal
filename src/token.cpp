#include "token.hpp"

Token::Token(TokenTypes type, Operator op) {
    this->type = type;
    this->op = op;
}

Token::Token(TokenTypes type, char* iden) {
    this->type = type;
    this->iden = iden;
}

Token::Token(TokenTypes type, Separator sep) {
    this->type = type;
    this->sep = sep;
}

Token::Token(TokenTypes type, int32_t intLit) {
    this->type = type;
    this->intLit = intLit;
}

Token::Token(TokenTypes type, float floatLit) {
    this->type = type;
    this->floatLit = floatLit;
}