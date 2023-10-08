#include "interpreter.hpp"

#include <unordered_map>

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

	Value()        { type = INT;   intVal   = 0; }
	Value(int i)   { type = INT;   intVal   = i; }
	Value(float f) { type = FLOAT; floatVal = f; }
	Value(bool b)  { type = BOOL;  boolVal  = b; }

	Value operator+(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() + other.get_scalar());
		else
			return Value(get_int() + get_int());
	}

	Value operator-(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() - other.get_scalar());
		else
			return Value(get_int() - get_int());
	}

	Value operator*(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() * other.get_scalar());
		else
			return Value(get_int() * get_int());
	}

	Value operator/(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(get_scalar() / other.get_scalar());
		else
			return Value(get_int() / get_int());
	}

	Value operator%(const Value& other)
	{
		if(type == FLOAT || other.type == FLOAT)
			return Value(fmodf(get_scalar(), other.get_scalar()));
		else
			return Value(get_int() % get_int());
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

Value evaluate_function(Function* func, const std::vector<Value>& args);
Value evaluate_expression(Expression* exp, const std::unordered_map<std::string, Value> params);

//------------------------------------------------------

std::string run(AST* ast, const std::vector<std::string>& args)
{
	return "run";
}

//------------------------------------------------------

Value evaluate_function(Function* func, const std::vector<Value>& args)
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
		Value condResult = evaluate_expression(func->map[i].second, params);
		if(condResult.type != Value::BOOL)
			throw new RuntimeErrorInvalidCondition(func->map[i].second->line, func->map[i].second->charIdx);
		
		if(condResult.boolVal)
			return evaluate_expression(func->map[i].first, params);
	}

	return Value(0);
}

Value evaluate_expression(Expression* exp, const std::unordered_map<std::string, Value> params)
{
	switch(exp->type)
	{
	case Expression::OPERATOR:
	{
		Value l = evaluate_expression(exp->op.left , params);
		Value r = evaluate_expression(exp->op.right, params);

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
    case Expression::VARIABLE:
    case Expression::INT_LITERAL:
    case Expression::FLOAT_LITERAL:
	default:
		throw new RuntimeErrorInvalidExpression(exp->line, exp->charIdx);
	}
}