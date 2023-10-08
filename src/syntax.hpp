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
    EQUALITY,
    GREATER,
    LESS,
    GREATEREQ,
    LESSEQ,
    EXP,
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
    COMMA,
    COLON
};

const std::unordered_map<std::string, Operator> OPERATORS = {
    {"+", ADD},
    {"-", SUB},
    {"*", MULT},
    {"/", DIV},
    {"%", MOD},
    {"=", EQUALITY},
    {">", GREATER},
    {"<", LESS},
    {">=", GREATEREQ},
    {"<=", LESSEQ},
    {"^", EXP},
    {"fn ", FN},
    {"of ", OF},
    {"otherwise", OTHERWISE}
};

const std::unordered_map<std::string, Separator> SEPARATORS = {
    {"{", OPEN_CURLY},
    {"}", CLOSE_CURLY},
    {"(", OPEN_PAREN},
    {")", CLOSE_PAREN},
    {",", COMMA},
    {":", COLON}
};

const std::unordered_map<Operator, int32_t> ORDER_OF_OPERATIONS = {
    {ADD, 1},
    {SUB, 1},
    {MULT, 2},
    {DIV, 2},
    {MOD, 2},
    {EQUALITY, 0},
    {GREATER, 0},
    {LESS, 0},
    {GREATEREQ, 0},
    {LESSEQ, 0},
    {EXP, 3}
};

#endif