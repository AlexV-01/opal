#ifndef OPAL_TOKEN_H
#define OPAL_TOKEN_H

#include <string>
#include <stdint.h>
#include "syntax.hpp"

struct Token
{
	enum Type
	{  
		OPERATOR,
		IDENTIFIER,
		SEPARATOR,
		NEWLINE,
		INT_LITERAL,
		FLOAT_LITERAL,

		EMPTY
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

	Token(Type type, Operator op);
	Token(Type type, char* iden);
	Token(Type type, Separator sep);
	Token(Type type, int32_t intLit);
	Token(Type type, float floatLit);
	Token(Type type);
};

#endif