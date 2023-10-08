fn num of n {
    1 : n = 0
    num(n) * num (n-1) : otherwise
}

fn den of n r {
    1 : (n-r) = 0
    den(n-r) * den((n-r)-1) : otherwise
}

fn perm of n r {
    num(n) / den(n r) : otherwise
}

fn main of n r {
    perm(n r)
}