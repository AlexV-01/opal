#include "token.hpp"

Token::Token(Token::Type type, Operator op, int32_t line, int32_t charIdx) : Token(type, line, charIdx) {
    this->op = op;
}

Token::Token(Token::Type type, char* iden, int32_t line, int32_t charIdx) : Token(type, line, charIdx) {
    this->iden = iden;
}

Token::Token(Token::Type type, Separator sep, int32_t line, int32_t charIdx) : Token(type, line, charIdx) {
    this->sep = sep;
}

Token::Token(Token::Type type, int32_t intLit, int32_t line, int32_t charIdx) : Token(type, line, charIdx) {
    this->intLit = intLit;
}

Token::Token(Token::Type type, float floatLit, int32_t line, int32_t charIdx) : Token(type, line, charIdx) {
    this->floatLit = floatLit;
}

Token::Token(Token::Type type, int32_t line, int32_t charIdx) {
    this->type = type;
    this->line = line;
    this->charIdx = charIdx;
}