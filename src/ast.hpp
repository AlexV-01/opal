#ifndef OPAL_AST_H
#define OPAL_AST_H

#include "syntax.hpp"
#include <vector>

typedef size_t ExpressionHandle;

struct Expression
{
    enum Type
    {
        OPERATOR,
        FUNCTION,
        VARIABLE,
        INT_LITERAL,
        FLOAT_LITERAL
    } type;

    union
    {
        struct
        {
            Operator op;
            int32_t parenDepth;
            ExpressionHandle left;
            ExpressionHandle right;
        } op;

        struct
        {
            char* name;
            int32_t numParams;
            ExpressionHandle* params;
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
    };

    int32_t line;
    int32_t charIdx;

    Expression(int32_t l, int32_t c)
    {
        line = l;
        charIdx = c;
    }
};

struct Function
{
    std::string name;
    std::vector<std::string> params;
    std::vector<std::pair<ExpressionHandle, ExpressionHandle>> map;

    int32_t line;
};

struct AST
{
private:
    std::vector<Expression> expressionBuf;

public:
    std::vector<Function> functions;

    Expression& get_exp(ExpressionHandle i) { return expressionBuf[i]; }
    ExpressionHandle add_exp(Expression e) { expressionBuf.push_back(e); return expressionBuf.size() - 1; }
};

#endif