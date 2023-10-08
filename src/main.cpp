#include <stdio.h>
#include <vector>
#include "lexer.hpp"

void print_tokens(std::vector<Token> tokens) {
	for (Token t : tokens) {
		switch(t.type) {
			case Token::Type::OPERATOR:
				printf("%d\n", t.op);
			case Token::Type::SEPARATOR:
				printf("%d\n", t.sep);
			case Token::Type::INT_LITERAL:
				printf("%d\n", t.intLit);
			case Token::Type::FLOAT_LITERAL:
				printf("%f\n", t.floatLit);
			case Token::Type::IDENTIFIER:
				printf("%s\n", t.iden);
			case Token::Type::NEWLINE:
				printf("NEW LINE HERE\n");
		}
	}
}

int main()
{

	std::vector<Token> tokens = lex_file("testfile.txt");
	print_tokens(tokens);
	free_tokens(tokens);
	return 0;
}