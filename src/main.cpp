#include <stdio.h>
#include <vector>
#include <iostream>
#include <exception>

#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

#define VERSION 0.1

int main(int argc, char *argv[])
{
	if (argc == 2 && argv[1] == "--version")
	{
		printf("%s", VERSION);
		return 0;
	}

	if(argc < 2)
		return -1;

	std::string fileName(argv[1]);
	fileName += ".opal";
	std::vector<std::string> args;
	for(int i = 2; i < argc; i++)
		args.push_back(std::string(argv[i]));

	try
	{
		std::vector<Token> tokens = lex_file(fileName);
		AST* ast = generate_ast(tokens);
		std::cout << run(ast, args) << std::endl;

		free_ast(ast);
		free_tokens(tokens);
	}
	catch(std::exception *e)
	{
		std::cout << e->what() << std::endl;
	}

	return 0;
}