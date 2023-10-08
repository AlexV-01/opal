#include "interpreter.hpp"
#include <math.h>

#include <unordered_map>
#include <algorithm>

//------------------------------------------------------
//base runtime error:

class RuntimeError : public std::exception 
{
private:
    int32_t line;
    int32_t charIdx;

protected:
    std::string get_position_str()
    {
        return "line " + std::to_string(line) + ":" + std::to_string(charIdx) + " - ";
    }

public:
    RuntimeError(int32_t l, int32_t c) : std::exception()
    {
        l = line;
        c = charIdx;
    }
};

//------------------------------------------------------
//specific runtime errors:

class RuntimeErrorFuncNotFound : public RuntimeError 
{ 
private:
	std::string name;

public:
    RuntimeErrorFuncNotFound(std::string n, int32_t l, int32_t c) : RuntimeError(l, c) { name = n; }
    std::string what() { return get_position_str() + "no function \"" + name + "\" found"; } 
};

class RuntimeErrorIncorrectNumArgs : public RuntimeError 
{ 
private:
	std::string name;
	int32_t num;

public:
    RuntimeErrorIncorrectNumArgs(std::string i, int32_t n, int32_t l, int32_t c) : RuntimeError(l, c) { name = i; num = n; }
    std::string what() { return get_position_str() + "no overload of function \"" + name + "\" takes " + std::to_string(num) + " arguments"; } 
};

class RuntimeErrorInvalidCondition : public RuntimeError
{
public:
    RuntimeErrorInvalidCondition(int32_t l, int32_t c) : RuntimeError(l, c) { }
    std::string what() { return get_position_str() + "invalid condition"; } 
};

class RuntimeErrorInvalidOperator : public RuntimeError
{
public:
    RuntimeErrorInvalidOperator(int32_t l, int32_t c) : RuntimeError(l, c) { }
    std::string what() { return get_position_str() + "invalid operator"; } 
};

class RuntimeErrorInvalidExpression : public RuntimeError
{
public:
    RuntimeErrorInvalidExpression(int32_t l, int32_t c) : RuntimeError(l, c) { }
    std::string what() { return get_position_str() + "invalid expression"; } 
};

class RuntimeErrorInvalidVariable : public RuntimeError
{
public:
	RuntimeErrorInvalidVariable(int32_t l, int32_t c) : RuntimeError(l, c) { }
	std::string what() { return get_position_str() + "invalid variable"; }
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
		int intVal;
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

	int get_int() const
	{
		switch(type)
		{
		case INT:
			return (int)intVal;
			break;
		case BOOL:
			return (int)boolVal;
			break;
		case FLOAT:
			return (int)floorf(floatVal);
		}
	}

	Value(int i)   { type = INT;   intVal   = i; }
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
			int result = 1;
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
Value evaluate_expression(Expression* exp, const std::unordered_map<std::string, Value>& params, AST* ast);

//------------------------------------------------------

std::string run(AST* ast, const std::vector<std::string>& args)
{
	for (Function f : ast->functions)
	{
		if (f.name == "main")
		{
			std::vector<Value> values;
			for (std::string param : f.params)
			{
				try
				{
					std::stoi(param);
					values.push_back(Value(std::stoi(param)));
				}
				catch (std::string param)
				{
					values.push_back(Value(std::stof(param)));
				}
			}
			evaluate_function(&f, values, ast);
		}
	}
	return "run";
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
			throw new RuntimeErrorInvalidCondition(func->map[i].second->line, func->map[i].second->charIdx);
		
		if(condResult.boolVal)
			return evaluate_expression(func->map[i].first, params, ast);
	}

	return Value(0);
}

Value evaluate_expression(Expression* exp, const std::unordered_map<std::string, Value>& params, AST* ast)
{
	switch(exp->type)
	{
	case Expression::OPERATOR:
	{
		Value l = evaluate_expression(exp->op.left , params, ast);
		Value r = evaluate_expression(exp->op.right, params, ast);

		switch(exp->op.op)
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
			throw new RuntimeErrorInvalidOperator(exp->line, exp->charIdx);
		}
	}
	case Expression::FUNCTION:
	{
		if (std::find(ast->functions.begin(), ast->functions.end(), exp->func.name) == ast->functions.end())
		{
			throw new RuntimeErrorFuncNotFound(exp->func.name, exp->line, exp->charIdx);
		}
		Function* funct;
		for (int i = 0; i < ast->functions.size(); i++)
		{
			if (ast->functions[i].name == exp->func.name)
			{
				funct = &ast->functions[i];
			}
		}
		std::vector<Value> values;
		for (int i = 0; i < exp->func.numParams; i++) {
			values.push_back(evaluate_expression(exp->func.params[i], params, ast));
		}
		return evaluate_function(funct, values, ast);
	}
    case Expression::VARIABLE:
	{
		if (params.find(exp->var.name) == params.end())
		{
			throw new RuntimeErrorInvalidVariable(exp->line, exp->charIdx);
		}
		return params.at(exp->var.name);
	}
    case Expression::INT_LITERAL:
		return Value(exp->intLit.val);
    case Expression::FLOAT_LITERAL:
		return Value(exp->floatLit.val);
	default:
		throw new RuntimeErrorInvalidExpression(exp->line, exp->charIdx);
	}
}