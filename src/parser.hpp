#ifndef OPAL_PARSER_H
#define OPAL_PARSER_H

#include "ast.hpp"
#include "token.hpp"
#include <vector>

AST* parse_tokens(std::vector<Token>& tokens);
void free_ast(AST* ast);

#endif