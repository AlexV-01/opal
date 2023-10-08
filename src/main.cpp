#include <stdio.h>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

int main()
{
	std::vector<Token> tokens = lex_file("testfile.txt");
	AST* ast = generate_ast(tokens);
	run(ast )
	free_tokens(tokens);
	return 0;
}