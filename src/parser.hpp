#ifndef OPAL_PARSER_H
#define OPAL_PARSER_H

#include "ast.hpp"
#include "token.hpp"
#include <vector>

AST* generate_ast(std::vector<Token>& tokens);
void free_ast(AST* ast);

SingleAST* generate_single_expression_ast(std::vector<Token>& tokens);
void free_single_expression_ast(SingleAST* expression);

#endif