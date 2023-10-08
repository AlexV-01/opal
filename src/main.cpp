#include <stdio.h>
#include <vector>
#include <iostream>
#include <exception>

#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

int main(int argc, char *argv[])
{
	if(argc < 2)
		return -1;

	std::string fileName(argv[0]);
	std::string mainFunc(argv[1]); 
	std::vector<std::string> args;
	for(int i = 2; i < argc; i++)
		args.push_back(std::string(argv[i]));

	try
	{
		std::vector<Token>& tokens = lex_file(fileName);
		AST* ast = generate_ast(tokens);
		std::cout << run(ast, args) << std::endl;

		free_ast(ast);
		free_tokens(tokens);
	}
	catch(std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}