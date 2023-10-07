#ifndef OPAL_AST_H
#define OPAL_AST_H

#include "syntax.hpp"

struct Expression
{
    enum Type
    {
        OPERATOR,
        FUNCTION,
        VARIABLE,
        INT_LITERAL,
        FLOAT_LITERAL,
        PAREN
    } type;

    union
    {
        struct
        {
            Operator op;
            int32_t parenDepth;
            Expression* left;
            Expression* right;
        } op;

        struct
        {
            char* name;
            int32_t numParams;
            Expression** params;
        } func;

        struct
        {
            char* name;
        } var;

        struct
        {
            int32_t val;
        } intLit;

        struct
        {
            float val;
        } floatLit;

        struct
        {
            Expression* exp;
        } paren;
    };
};

struct Function
{
    std::string name;
    std::vector<std::string> params;
    std::vector<std::pair<Expression*, Expression*>> map;

    int32_t line;
};

struct AST
{
    std::vector<Function> functions;
    std::vector<Expression> expressionBuf;
};

#endif