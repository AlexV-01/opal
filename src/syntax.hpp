#ifndef OPAL_SYNTAX_H
#define OPAL_SYNTAX_H

#include <unordered_map>
#include <string>

enum Operator
{
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,

    FN,
    OF,
    OTHERWISE
};

enum Separator
{
    OPEN_CURLY,
    CLOSE_CURLY,
    OPEN_PAREN,
    CLOSE_PAREN,
    ARROW
};

const std::unordered_map<std::string, Operator> OPERATORS = {
    {"+", ADD},
    {"-", SUB},
    {"*", MULT},
    {"/", DIV},
    {"%", MOD},
    {"fn", FN},
    {"of", OF},
    {"otherwise", OTHERWISE}
};

const std::unordered_map<std::string, Separator> SEPARATORS = {
    {"{", OPEN_CURLY},
    {"}", CLOSE_CURLY},
    {"(", OPEN_PAREN},
    {")", CLOSE_PAREN},
    {"->", ARROW}
};

const std::unordered_map<Operator, int32_t> ORDER_OF_OPERATIONS = {
    {ADD, 0},
    {SUB, 0},
    {MULT, 1},
    {DIV, 1},
    {MOD, 1}
};

#endif