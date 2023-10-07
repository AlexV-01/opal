#ifndef OPAL_TOKEN_H
#define OPAL_TOKEN_H

#include <string>
#include <stdint.h>
#include "syntax.hpp"

struct Token
{
	enum TokenTypes
	{  
		OPERATOR,
		IDENTIFIER,
		SEPARATOR,
		NEWLINE,
		INT_LITERAL,
		FLOAT_LITERAL
	} type;

	union
	{
		Operator op;
		char* iden;
		Separator sep;
		int32_t intLit;
		float floatLit;
	};

	int32_t line;
	int32_t charIdx;

	Token(TokenTypes type, Operator op);
	Token(TokenTypes type, char* iden);
	Token(TokenTypes type, Separator sep);
	Token(TokenTypes type, int32_t intLit);
	Token(TokenTypes type, float floatLit);
};

#endif