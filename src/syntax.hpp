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

#endif