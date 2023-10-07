#ifndef OPAL_TOKEN_H
#define OPAL_TOKEN_H

#include <string>
#include <stdint.h>

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
		std::string op;
		std::string iden;
		std::string sep;
		int32_t intLit;
		float floatLit;
	}; 
};

#endif