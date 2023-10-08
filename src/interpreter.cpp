#include "interpreter.hpp"
#include <math.h>

#include <unordered_map>
#include <algorithm>
#include <iostream>

//------------------------------------------------------
//base runtime error:

class RuntimeError : public std::exception 
{
protected:
    std::string str;

public:
    RuntimeError(int32_t line, int32_t charIdx) : std::exception()
    {
        str = "line " + std::to_string(line) + ":" + std::to_string(charIdx) + " - ";
    }

    const char* what() const noexcept override
    {
        return str.c_str();
    }
};

//------------------------------------------------------
//specific runtime errors:

class RuntimeErrorFuncNotFound : public RuntimeError 
{ 
public:
    RuntimeErrorFuncNotFound(std::string n, int32_t l, int32_t c) : RuntimeError(l, c) { str += "no function \"" + n + "\" found"; }
};

class RuntimeErrorIncorrectNumArgs : public RuntimeError 
{ 
public:
    RuntimeErrorIncorrectNumArgs(std::string i, int32_t n, int32_t l, int32_t c) : RuntimeError(l, c) { str += "no overload of function \"" + i + "\" takes " + std::to_string(n) + " arguments"; }
};

class RuntimeErrorInvalidCondition : public RuntimeError
{
public:
    RuntimeErrorInvalidCondition(int32_t l, int32_t c) : RuntimeError(l, c) { str += "invalid condition"; }
};

class RuntimeErrorInvalidOperator : public RuntimeError
{
public:
    RuntimeErrorInvalidOperator(int32_t l, int32_t c) : RuntimeError(l, c) { str += "invalid operator"; }
};

class RuntimeErrorInvalidExpression : public RuntimeError
{
public:
    RuntimeErrorInvalidExpression(int32_t l, int32_t c) : RuntimeError(l, c) { str += "invalid expression"; }
};

class RuntimeErrorInvalidVariable : public RuntimeError
{
public:
	RuntimeErrorInvalidVariable(int32_t l, int32_t c) : RuntimeError(l, c) { str += "invalid variable"; }
};

//------------------------------------------------------

struct Value
{
	enum Type
	{
		INT,
		FLOAT,
		BOOL
	} type;

	union
	{
		float floatVal;
		int64_t intVal;
		bool boolVal;
	};

	float get_scalar() const
	{
		switch(type)
		{
		case INT:
			return (float)intVal;
			break;
		case BOOL:
			return (float)boolVal;
			break;
		case FLOAT:
			return (float)floatVal;
		}
	}

	int64_t get_int() const
	{
		switch(type)
		{
		case INT:
			return (int64_t)intVal;
			break;
		case BOOL:
			return (int64_t)boolVal;
			break;
		case FLOAT:
			return (int64_t)floorf(floatVal);
		}
	}

	Value()        { type = INT;   intVal   = 0; }
	Value(int64_t i)   { type = INT;   intVal   = i; }
	Value(float f) { type = FLOAT; floatVal = f; }
	Value(bool b)  { type = BOOL;  boolVal  = b; }

	Value operator+(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() + other.get_scalar());
		else
			return Value(get_int() + other.get_int());
	}

	Value operator-(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() - other.get_scalar());
		else
			return Value(get_int() - other.get_int());
	}

	Value operator*(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() * other.get_scalar());
		else
			return Value(get_int() * other.get_int());
	}

	Value operator/(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() / other.get_scalar());
		else
			return Value(get_int() / other.get_int());
	}

	Value operator%(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(fmodf(get_scalar(), other.get_scalar()));
		else
			return Value(get_int() % other.get_int());
	}

	Value operator==(const Value& other)
	{
		return Value(get_scalar() == other.get_scalar());
	}

	Value operator>(const Value& other)
	{
		return Value(get_scalar() > other.get_scalar());
	}

	Value operator<(const Value& other)
	{
		return Value(get_scalar() < other.get_scalar());
	}

	Value operator>=(const Value& other)
	{
		return Value(get_scalar() >= other.get_scalar());
	}

	Value operator<=(const Value& other)
	{
		return Value(get_scalar() <= other.get_scalar());
	}

	Value to(const Value& other)
	{
		if(type != FLOAT && other.type != FLOAT)
		{
			int64_t result = 1;
			for(int i = 0; i < other.get_int(); i++)
				result *= get_int();

			return Value(result);
		}
		else
			return Value(powf(get_scalar(), other.get_scalar()));
	}
};

//------------------------------------------------------

//------------------------------------------------------

Value evaluate_function(Function* func, const std::vector<Value>& args, AST* ast);
Value evaluate_expression(ExpressionHandle exp, const std::unordered_map<std::string, Value>& params, AST* ast);

//------------------------------------------------------

std::string run(AST* ast, std::vector<std::string> args)
{
	for (Function f : ast->functions)
	{
		if (f.name == "main")
		{
			std::vector<Value> values;
			if (args.size() != f.params.size())
				return "args size mismatch eror";

			for (int i = 0; i < args.size(); i++)
			{
				try
				{
					std::stoi(args[i]);
					values.push_back(Value((int64_t)std::stoi(args[i])));
				}
				catch (std::exception e)
				{
					values.push_back(Value(std::stof(args[i])));
				}
			}

			Value result = evaluate_function(&f, values, ast);
			switch (result.type)
			{
			case Value::FLOAT:
				return std::to_string(result.floatVal);
			case Value::INT:
				return std::to_string(result.intVal);
			case Value::BOOL:
				return std::to_string(result.boolVal);
			}
		}
	}

	return "unknown error";
}

//------------------------------------------------------

Value evaluate_function(Function* func, const std::vector<Value>& args, AST* ast)
{
	//setup params:
	//----------------
	if(args.size() != func->params.size())
		throw new RuntimeErrorIncorrectNumArgs(func->name, args.size(), func->line, 0);
	
	std::unordered_map<std::string, Value> params;
	for(int i = 0; i < args.size(); i++)
		params[func->params[i]] = args[i];
	
	//find correct expression to evaluate by evaluating conditions:
	//----------------
	for(int i = 0; i < func->map.size(); i++)
	{
		Value condResult = evaluate_expression(func->map[i].second, params, ast);
		if(condResult.type != Value::BOOL)
			throw new RuntimeErrorInvalidCondition(ast->get_exp(func->map[i].second).line, ast->get_exp(func->map[i].second).charIdx);
		
		if(condResult.boolVal)
			return evaluate_expression(func->map[i].first, params, ast);
	}

	return Value((int64_t)0);
}

Value evaluate_expression(ExpressionHandle exp, const std::unordered_map<std::string, Value>& params, AST* ast)
{
	switch(ast->get_exp(exp).type)
	{
	case Expression::OPERATOR:
	{
		Value l, r;
		if (ast->get_exp(exp).op.op != OTHERWISE)
		{
			l = evaluate_expression(ast->get_exp(exp).op.left, params, ast);
			r = evaluate_expression(ast->get_exp(exp).op.right, params, ast);
		}

		switch(ast->get_exp(exp).op.op)
		{
		case ADD:
			return l + r;
		case SUB:
			return l - r;
		case MULT:
			return l * r;
		case DIV:
			return l / r;
		case MOD:
			return l % r;
		case EXP:
			return l.to(r);
		case EQUALITY:
			return l == r;
		case LESS:
			return l < r;
		case GREATER:
			return l > r;
		case LESSEQ:
			return l <= r;
		case GREATEREQ:
			return l >= r;
		case OTHERWISE:
			return Value(true);
		default:
			throw new RuntimeErrorInvalidOperator(ast->get_exp(exp).line, ast->get_exp(exp).charIdx);
		}
	}
	case Expression::FUNCTION:
	{
		for (int i = 0; i < ast->functions.size(); i++)
		{
			if (ast->get_exp(exp).func.name == ast->functions[i].name)
			{
				Function* funct;
				for (int i = 0; i < ast->functions.size(); i++)
				{
					if (ast->functions[i].name == ast->get_exp(exp).func.name)
					{
						funct = &ast->functions[i];
					}
				}
				std::vector<Value> values;
				for (int i = 0; i < ast->get_exp(exp).func.numParams; i++)
				{
					values.push_back(evaluate_expression(ast->get_exp(exp).func.params[i], params, ast));
				}
				return evaluate_function(funct, values, ast);
			}
		}
		throw new RuntimeErrorFuncNotFound(ast->get_exp(exp).func.name, ast->get_exp(exp).line, ast->get_exp(exp).charIdx);
	}
    case Expression::VARIABLE:
	{
		if (params.find(ast->get_exp(exp).var.name) == params.end())
		{
			throw new RuntimeErrorInvalidVariable(ast->get_exp(exp).line, ast->get_exp(exp).charIdx);
		}
		return params.at(ast->get_exp(exp).var.name);
	}
    case Expression::INT_LITERAL:
		return Value((int64_t) ast->get_exp(exp).intLit.val);
    case Expression::FLOAT_LITERAL:
		return Value(ast->get_exp(exp).floatLit.val);
	default:
		throw new RuntimeErrorInvalidExpression(ast->get_exp(exp).line, ast->get_exp(exp).charIdx);
	}
}