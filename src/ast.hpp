#ifndef OPAL_AST_H
#define OPAL_AST_H

struct Expression
{
    enum Type
    {
        OPERATOR,
        FUNCTION,
        INT_LITERAL,
        FLOAT_LITERAL
    } type;

    union
    {
        struct
        {
            
        } op;
    };
};

struct Function
{
    std::
};

struct AST
{

};

#endif