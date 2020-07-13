int a = 5;
int u = 5 % 3;

int kuh(int x);
int abs(int x)
{
	if (x < 0) return -x;
	return x;
}

int BCD(int x, int y)
{
	if (x == 0) return y;
	if (y == 0) return x;
	if (x < 0 || y < 0) 
	{
		return BCD(abs(x), abs(y));
	}
	if (x < y) return BCD(x, y % x);
	return BCD(y, x % y);
}

int fib(int n)
{
	int x;
	if (n < 2) return 0;
	if (n == 2) return 1;
	return fib(n - 2) + fib(n - 2);
}

int main()
{
	return fib(15);
}

int kuh(int x)
{
	return x * 2;
}
