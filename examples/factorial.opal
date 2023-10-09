fn fact of n a
{
	-1 : n < 0
	1 : n = 0
	a : n = 1
	fact(n-1, a*n) : otherwise
}

fn main of n
{
	fact(n, 1)
}