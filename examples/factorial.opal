fn factorial of n
{
	0                    : n < 0
	1                    : n = 0
	n * factorial(n - 1) : otherwise
}

fn main of n
{
	factorial(n)
}
