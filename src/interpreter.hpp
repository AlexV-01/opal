#ifndef OPAL_INTERPRETER_H
#define OPAL_INTERPRETER_H

#include "ast.hpp"

std::string run(AST* ast, std::vector<std::string> args);

#endif