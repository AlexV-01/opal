#ifndef OPAL_LEXER_H
#define OPAL_LEXER_H

#include <vector>
#include <string>
#include "token.hpp"

std::vector<Token> lex_file(std::string fileName);

#endif