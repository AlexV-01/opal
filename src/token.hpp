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
		std::string iden;
		Separator sep;
		int32_t intLit;
		float floatLit;
	}; 
};

#endif