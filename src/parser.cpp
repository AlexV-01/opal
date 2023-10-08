#include "parser.hpp"
#include <iostream>
#include <string.h>
#include <stack>

//------------------------------------------------------
//base parse error:

class ParseError : public std::exception 
{
private:
    int32_t line;
    int32_t charIdx;

protected:
    std::string get_position_str()
    {
        return "line " + std::to_string(line) + ":" + std::to_string(charIdx) + " - ";
    }

public:
    ParseError(int32_t l, int32_t c) : std::exception()
    {
        l = line;
        c = charIdx;
    }
};

//------------------------------------------------------
//specific parse errors:

class ParseErrorExpectedFunction : public ParseError 
{ 
public:
    ParseErrorExpectedFunction(int32_t l, int32_t c) : ParseError(l, c) { }
    std::string what() { return get_position_str() + "expected a function"; } 
};

class ParseErrorExpectedIdentifier : public ParseError 
{ 
public:
    ParseErrorExpectedIdentifier(int32_t l, int32_t c) : ParseError(l, c) { }
    std::string what() { return get_position_str() + "expected an identifier"; } 
};

class ParseErrorExpectedSeparator : public ParseError 
{ 
public:
    ParseErrorExpectedSeparator(int32_t l, int32_t c) : ParseError(l, c) { }
    std::string what() { return get_position_str() + "expected a separator"; } 
};

class ParseErrorExpectedOperator : public ParseError 
{ 
public:
    ParseErrorExpectedOperator(int32_t l, int32_t c) : ParseError(l, c) { }
    std::string what() { return get_position_str() + "expected an operator"; } 
};

class ParseErrorFunctionRedef : public ParseError 
{
private:
    std::string funcName;

public:
    ParseErrorFunctionRedef(std::string n, int32_t l, int32_t c) : ParseError(l, c) { funcName = n; }
    std::string what() { return get_position_str() + "\"" + funcName + "\" function redefinition"; } 
};

class ParseErrorParamRedef : public ParseError 
{
private:
    std::string paramName;

public:
    ParseErrorParamRedef(std::string n, int32_t l, int32_t c) : ParseError(l, c) { paramName = n; }
    std::string what() { return get_position_str() + "\"" + paramName + "\" parameter redefinition"; } 
};

//------------------------------------------------------
//static func declarations:

static Function parse_function(AST* ast, std::vector<Token>& tokens, size_t& pos);
static Expression* parse_expression(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth);

static Expression* parse_iden_lit(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth);
static Expression* parse_op(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth);

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
        return Token(Token::EMPTY);
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
    size_t pos;
    
    while(pos < tokens.size())
    {
        try
        {
            ast->functions.push_back(parse_function(ast, tokens, pos));
        }
        catch(ParseError error)
        {
            //TODO: proper error logging:
            std::cout << "opal parse error - " << error.what() << std::endl;
        }
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
        }

        pos--;
    }
    else
        pos--;

    //parse expressions:
    //----------------
    Token openBrace = next_token_skip_newline(tokens, pos);
    if(openBrace.type != Token::SEPARATOR || openBrace.op != OPEN_CURLY)
        throw new ParseErrorExpectedSeparator(openBrace.line, openBrace.charIdx);
    
    Expression* firstExp = parse_expression(ast, tokens, pos, 0);
    Token firstExpEnd = next_token_skip_newline(tokens, pos);
    if(firstExpEnd.type == Token::SEPARATOR && firstExpEnd.sep == CLOSE_CURLY) //single case function
    {
        Expression otherwise;
        otherwise.type = Expression::OPERATOR;
        otherwise.op.op = OTHERWISE;

        ast->expressionBuf.push_back(otherwise);
        func.map.push_back({firstExp, &ast->expressionBuf.back()});
    }
    else if(firstExpEnd.type == Token::SEPARATOR && firstExpEnd.op == COLON) //multi case function
    {
        Expression* firstCond = parse_expression(ast, tokens, pos, 0);
        func.map.push_back({firstExp, firstCond});

        Token end = next_token_skip_newline(tokens, pos);
        while(end.type != Token::SEPARATOR || end.op != CLOSE_CURLY)
        {
            Expression* exp = parse_expression(ast, tokens, pos, 0);

            Token colon = next_token_skip_newline(tokens, pos);
            if(colon.type != Token::SEPARATOR || colon.op != COLON)
                throw new ParseErrorExpectedSeparator(colon.line, colon.charIdx);

            Expression* cond = parse_expression(ast, tokens, pos, 0);

            func.map.push_back({exp, cond});
        }
    }
    else
        throw new ParseErrorExpectedSeparator(firstExpEnd.line, firstExpEnd.charIdx);

    return func;
} 

static Expression* parse_expression(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    Token nextToken = next_token(tokens, pos, parenDepth);
    if(nextToken.type == Token::OPERATOR && nextToken.op == OTHERWISE)
    {
        Expression exp;
        exp.type = Expression::OPERATOR;
        exp.op.op = OTHERWISE;

        ast->expressionBuf.push_back(exp);
        return &ast->expressionBuf.back();
    }
    else
        pos--;

    //shunting yard:
    std::stack<Expression*> nums;
    std::stack<Expression*> ops;

    while(true)
    {
        nums.push(parse_iden_lit(ast, tokens, pos, parenDepth));
        
        Token nextToken = next_token(tokens, pos, parenDepth);
        if(nextToken.type != Token::OPERATOR)
            break;
        pos--;

        Expression* op = parse_op(ast, tokens, pos, parenDepth);
        if(ORDER_OF_OPERATIONS.at(op->op.op) <= ORDER_OF_OPERATIONS.at(ops.top()->op.op))
        {
            while(!ops.empty())
            {
                Expression* evaledOp = ops.top(); ops.pop();
                
                evaledOp->op.right = nums.top(); nums.pop();
                evaledOp->op.left = nums.top(); nums.pop();

                nums.push(evaledOp);
            }
        }

        ops.push(op);
    }

    while(!ops.empty())
    {
        Expression* evaledOp = ops.top(); ops.pop();
                
        evaledOp->op.right = nums.top(); nums.pop();
        evaledOp->op.left = nums.top(); nums.pop();

        nums.push(evaledOp);
    }

    return nums.top();
}

//------------------------------------------------------
//static func definitions:

static Expression* parse_iden_lit(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    Token token = next_token(tokens, pos, parenDepth);
    if(token.type == Token::SEPARATOR && token.sep == OPEN_PAREN) //in parenthesis
    {
        Expression* exp = parse_expression(ast, tokens, pos, parenDepth + 1);

        Token closeParen = next_token(tokens, pos, parenDepth + 1);
        if(closeParen.type != Token::SEPARATOR || token.sep != CLOSE_PAREN)
            throw new ParseErrorExpectedSeparator(closeParen.line, closeParen.charIdx);

        return exp;   
    }

    if(token.type == Token::OPERATOR && token.op == SUB) //multiplying by -1
    {
        Expression minus1;
        minus1.type = Expression::INT_LITERAL;
        minus1.intLit.val = -1;
        ast->expressionBuf.push_back(minus1);

        Expression mult;
        mult.type = Expression::OPERATOR;
        mult.op.op = MULT;
        mult.op.left = &ast->expressionBuf.back();
        mult.op.right = parse_iden_lit(ast, tokens, pos, parenDepth);

        ast->expressionBuf.push_back(mult);
        return &ast->expressionBuf.back();
    }

    Expression exp;
    switch(token.type)
    {
    case Token::IDENTIFIER:
    {
        Token nextToken = next_token(tokens, pos, parenDepth);
        if(nextToken.type == Token::SEPARATOR && nextToken.sep == OPEN_PAREN) //function
        {
            exp.type = Expression::FUNCTION;
            exp.func.name = new char[strlen(nextToken.iden)];
            strcpy(exp.var.name, nextToken.iden);

            std::vector<Expression*> params;
            nextToken = next_token(tokens, pos, parenDepth + 1);

            while(nextToken.type != Token::SEPARATOR || nextToken.sep != CLOSE_PAREN)
            {
                params.push_back(parse_expression(ast, tokens, pos, parenDepth + 1));
                nextToken = next_token(tokens, pos, parenDepth + 1);

                if(nextToken.type != Token::SEPARATOR || (nextToken.type != CLOSE_PAREN && nextToken.type != COMMA))
                    throw new ParseErrorExpectedSeparator(nextToken.line, nextToken.charIdx);
            }

            exp.func.numParams = params.size();
            exp.func.params = new Expression*[exp.func.numParams];
            memcpy(exp.func.params, params.data(), exp.func.numParams * sizeof(Expression*));
        }
        else //variable
        {
            pos--;

            exp.type = Expression::VARIABLE;
            exp.var.name = new char[strlen(nextToken.iden)];
            strcpy(exp.var.name, nextToken.iden);
        }
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

    ast->expressionBuf.push_back(exp);
    return &ast->expressionBuf.back();
}

static Expression* parse_op(AST* ast, std::vector<Token>& tokens, size_t& pos, int32_t parenDepth)
{
    Token token = next_token(tokens, pos, parenDepth);
    if(token.type != Token::OPERATOR || token.op == FN || token.op == OTHERWISE || token.op == OF)
        throw new ParseErrorExpectedOperator(token.line, token.charIdx);

    Expression exp;
    exp.type = Expression::OPERATOR;
    exp.op.op = token.op;

    ast->expressionBuf.push_back(exp);
    return &ast->expressionBuf.back();
}