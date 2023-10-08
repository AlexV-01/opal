#include "parser.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <stack>

//------------------------------------------------------
//base parse error:

class ParseError : public std::exception 
{
protected:
    std::string str;

public:
    ParseError(int32_t line, int32_t charIdx) : std::exception()
    {
        str = "line " + std::to_string(line) + ":" + std::to_string(charIdx) + " - ";
    }

    const char* what() const noexcept override
    {
        return str.c_str();
    }
};

//------------------------------------------------------
//specific parse errors:

class ParseErrorExpectedFunction : public ParseError 
{
public:
    ParseErrorExpectedFunction(int32_t l, int32_t c) : ParseError(l, c) { str += "expected a function"; }
};

class ParseErrorExpectedIdentifier : public ParseError 
{ 
public:
    ParseErrorExpectedIdentifier(int32_t l, int32_t c) : ParseError(l, c) { str += "expected an identifier"; }
};

class ParseErrorExpectedSeparator : public ParseError 
{ 
public:
    ParseErrorExpectedSeparator(int32_t l, int32_t c) : ParseError(l, c) { str += "expected a separator"; }
};

class ParseErrorExpectedOperator : public ParseError 
{ 
public:
    ParseErrorExpectedOperator(int32_t l, int32_t c) : ParseError(l, c) { str += "expected an operator"; }
};

class ParseErrorFunctionRedef : public ParseError 
{
public:
    ParseErrorFunctionRedef(std::string n, int32_t l, int32_t c) : ParseError(l, c) { str += "\"" + n + "\" function redefinition"; }
};

class ParseErrorParamRedef : public ParseError 
{
public:
    ParseErrorParamRedef(std::string n, int32_t l, int32_t c) : ParseError(l, c) { str += "\"" + n + "\" parameter redefinition"; }
};

//------------------------------------------------------
//static func declarations:

static Function parse_function(AST* ast, std::vector<Token>& tokens, size_t& pos);
static ExpressionHandle parse_expression(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth);

static ExpressionHandle parse_iden_lit(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth);
static ExpressionHandle parse_op(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth);

//------------------------------------------------------
//helper func definitions:

inline static void remove_newline_tokens(std::vector<Token>& tokens, size_t& pos)
{
    while(tokens[pos].type == Token::NEWLINE && pos < tokens.size())
        pos++;   
}

inline static Token next_token_keep_newline(std::vector<Token>& tokens, size_t& pos)
{
    if(pos >= tokens.size())
        return Token(Token::EMPTY, 0, 0);
    else
        return tokens[pos++];
}

inline static Token next_token_skip_newline(std::vector<Token>& tokens, size_t& pos)
{
    remove_newline_tokens(tokens, pos);
    return next_token_keep_newline(tokens, pos);
}

inline static Token next_token(std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    if(parenDepth == 0)
        return next_token_keep_newline(tokens, pos);
    else
        return next_token_skip_newline(tokens, pos);
}

//------------------------------------------------------
//non-static func definitions:

AST* generate_ast(std::vector<Token>& tokens)
{
    AST* ast = new AST;
    size_t pos = 0;
    
    remove_newline_tokens(tokens, pos);

    while(pos < tokens.size())
    {
        ast->functions.push_back(parse_function(ast, tokens, pos));
        remove_newline_tokens(tokens, pos);
    }

    return ast;
}

void free_ast(AST* ast)
{
    //TODO: free strings + arrays
    delete ast;
}

//------------------------------------------------------
//static func definitions:

static Function parse_function(AST* ast, std::vector<Token>& tokens, size_t& pos)
{
    Function func;

    //ensure function is declared properly:
    //----------------
    Token declare = next_token_skip_newline(tokens, pos);
    if(declare.type != Token::OPERATOR || declare.op != FN)
        throw new ParseErrorExpectedFunction(declare.line, declare.charIdx);
    
    func.line = declare.line;

    //ensure name is an idenfifier and is unique:
    //----------------
    Token name = next_token_skip_newline(tokens, pos);
    if(name.type != Token::IDENTIFIER)
        throw new ParseErrorExpectedIdentifier(name.line, name.charIdx);

    for(int32_t i = 0; i < ast->functions.size(); i++)
        if(strcmp(name.iden, ast->functions[i].name.c_str()) == 0)
            throw new ParseErrorFunctionRedef(ast->functions[i].name, name.line, name.charIdx);

    func.name = std::string(name.iden);

    //parse arguments (if any)
    //----------------
    Token of = next_token_skip_newline(tokens, pos);
    if(of.type == Token::OPERATOR && of.op == OF)
    {
        Token param = next_token_skip_newline(tokens, pos);
        while(param.type == Token::IDENTIFIER)
        {
            for(int i = 0; i < func.params.size(); i++)
                if(strcmp(name.iden, func.params[i].c_str()) == 0)
                    throw new ParseErrorParamRedef(func.params[i].c_str(), param.line, param.charIdx);
            
            func.params.push_back(std::string(param.iden));
            param = next_token_skip_newline(tokens, pos);
        }

        pos--;
    }
    else
        pos--;

    //parse expressions:
    //----------------
    Token openBrace = next_token_skip_newline(tokens, pos);
    if(openBrace.type != Token::SEPARATOR || openBrace.sep != OPEN_CURLY)
        throw new ParseErrorExpectedSeparator(openBrace.line, openBrace.charIdx);
    
    ExpressionHandle firstExp = parse_expression(ast, tokens, pos, 0);
    Token firstExpEnd = next_token_skip_newline(tokens, pos);
    if(firstExpEnd.type == Token::SEPARATOR && firstExpEnd.sep == CLOSE_CURLY) //single case function
    {
        Expression otherwise(firstExpEnd.line, firstExpEnd.charIdx);
        otherwise.type = Expression::OPERATOR;
        otherwise.op.op = OTHERWISE;

        func.map.push_back({firstExp, ast->add_exp(otherwise)});
    }
    else if(firstExpEnd.type == Token::SEPARATOR && firstExpEnd.sep == COLON) //multi case function
    {
        ExpressionHandle firstCond = parse_expression(ast, tokens, pos, 0);
        func.map.push_back({firstExp, firstCond});

        Token end = next_token_skip_newline(tokens, pos);
        while(end.type != Token::SEPARATOR || end.sep != CLOSE_CURLY)
        {
            pos--;
            ExpressionHandle exp = parse_expression(ast, tokens, pos, 0);

            Token colon = next_token_skip_newline(tokens, pos);
            if(colon.type != Token::SEPARATOR || colon.sep != COLON)
                throw new ParseErrorExpectedSeparator(colon.line, colon.charIdx);

            ExpressionHandle cond = parse_expression(ast, tokens, pos, 0);

            func.map.push_back({exp, cond});
            end = next_token_skip_newline(tokens, pos);
        }
    }
    else
        throw new ParseErrorExpectedSeparator(firstExpEnd.line, firstExpEnd.charIdx);

    return func;
} 

static ExpressionHandle parse_expression(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    remove_newline_tokens(tokens, pos);

    Token nextToken = next_token(tokens, pos, parenDepth);
    if(nextToken.type == Token::OPERATOR && nextToken.op == OTHERWISE)
    {
        Expression exp(nextToken.line, nextToken.charIdx);
        exp.type = Expression::OPERATOR;
        exp.op.op = OTHERWISE;

        return ast->add_exp(exp);
    }
    else
        pos--;

    //shunting yard:
    std::stack<ExpressionHandle> nums;
    std::stack<ExpressionHandle> ops;

    while(true)
    {
        nums.push(parse_iden_lit(ast, tokens, pos, parenDepth));
        
        Token nextToken = next_token(tokens, pos, parenDepth); pos--;
        if(nextToken.type != Token::OPERATOR)
            break;

        ExpressionHandle op = parse_op(ast, tokens, pos, parenDepth);
        if(!ops.empty() && ORDER_OF_OPERATIONS.at(ast->get_exp(op).op.op) <= ORDER_OF_OPERATIONS.at(ast->get_exp(ops.top()).op.op))
        {
            while(!ops.empty())
            {
                ExpressionHandle evaledOp = ops.top(); ops.pop();
                
                ast->get_exp(evaledOp).op.right = nums.top(); nums.pop();
                ast->get_exp(evaledOp).op.left = nums.top(); nums.pop();

                nums.push(evaledOp);
            }
        }

        ops.push(op);
    }

    while(!ops.empty())
    {
        ExpressionHandle evaledOp = ops.top(); ops.pop();
                
        ast->get_exp(evaledOp).op.right = nums.top(); nums.pop();
        ast->get_exp(evaledOp).op.left = nums.top(); nums.pop();

        nums.push(evaledOp);
    }

    return nums.top();
}

//------------------------------------------------------
//static func definitions:

static ExpressionHandle parse_iden_lit(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    Token token = next_token(tokens, pos, parenDepth);
    if(token.type == Token::SEPARATOR && token.sep == OPEN_PAREN) //in parenthesis
    {
        ExpressionHandle exp = parse_expression(ast, tokens, pos, parenDepth + 1);

        Token closeParen = next_token(tokens, pos, parenDepth + 1);
        if(closeParen.type != Token::SEPARATOR || token.sep != CLOSE_PAREN)
            throw new ParseErrorExpectedSeparator(closeParen.line, closeParen.charIdx);

        return exp;   
    }

    if(token.type == Token::OPERATOR && token.op == SUB) //multiplying by -1
    {
        Expression minus1(token.line, token.charIdx);
        minus1.type = Expression::INT_LITERAL;
        minus1.intLit.val = -1;
        ExpressionHandle hMinus1 = ast->add_exp(minus1);

        Expression mult(token.line, token.charIdx);
        mult.type = Expression::OPERATOR;
        mult.op.op = MULT;
        mult.op.left = hMinus1;
        mult.op.right = parse_iden_lit(ast, tokens, pos, parenDepth);

        return ast->add_exp(mult);
    }

    Expression exp(token.line, token.charIdx);
    switch(token.type)
    {
    case Token::IDENTIFIER:
    {
        Token nextToken = next_token(tokens, pos, parenDepth);
        if(nextToken.type == Token::SEPARATOR && nextToken.sep == OPEN_PAREN) //function
        {
            exp.type = Expression::FUNCTION;
            exp.func.name = new char[strlen(token.iden)];
            strcpy(exp.var.name, token.iden);

            std::vector<ExpressionHandle> params;

            while(nextToken.type != Token::SEPARATOR || nextToken.sep != CLOSE_PAREN)
            {
                params.push_back(parse_expression(ast, tokens, pos, parenDepth + 1));
                nextToken = next_token(tokens, pos, parenDepth + 1);

                if(nextToken.type != Token::SEPARATOR || (nextToken.sep != CLOSE_PAREN && nextToken.sep != COMMA))
                    throw new ParseErrorExpectedSeparator(nextToken.line, nextToken.charIdx);
            }

            exp.func.numParams = params.size();
            exp.func.params = new ExpressionHandle[exp.func.numParams];
            memcpy(exp.func.params, params.data(), exp.func.numParams * sizeof(ExpressionHandle));
        }
        else //variable
        {
            pos--;

            exp.type = Expression::VARIABLE;
            exp.var.name = new char[strlen(token.iden)];
            strcpy(exp.var.name, token.iden);
        }

        break;
    }
    case Token::INT_LITERAL:
    {
        exp.type = Expression::INT_LITERAL;
        exp.intLit.val = token.intLit;

        break;
    }
    case Token::FLOAT_LITERAL:
    {
        exp.type = Expression::FLOAT_LITERAL;
        exp.floatLit.val = token.floatLit;

        break;
    }
    default:
        throw new ParseErrorExpectedIdentifier(token.line, token.charIdx);
    }

    return ast->add_exp(exp);
}

static ExpressionHandle parse_op(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    Token token = next_token(tokens, pos, parenDepth);
    if(token.type != Token::OPERATOR || token.op == FN || token.op == OTHERWISE || token.op == OF)
        throw new ParseErrorExpectedOperator(token.line, token.charIdx);

    Expression exp(token.line, token.charIdx);
    exp.type = Expression::OPERATOR;
    exp.op.op = token.op;

    return ast->add_exp(exp);
}