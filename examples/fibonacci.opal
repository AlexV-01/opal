fn fibonacci of n prev cur {
	-1 : cur < 0
	0 : n = 0
	1 : n = 1
	cur : n = 2
	fibonacci(n-1, cur, prev+cur) : otherwise
}

fn main of n {
	fibonacci(n, 1, 1)
}