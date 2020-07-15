int a;

int kuh(int x);
int abs(int x)
{
	int y = x + y;
	if (x < 0)
		return -x;
	return x;
}

int BCD(int x, int y)
{
	if (x == 0)
		return y;
	if (y == 0)
		return x;
	if (x < 0 || y < 0)
	{
		return BCD(abs(x), abs(y));
	}
	if (x < y)
		return BCD(x, y % x);
	return BCD(y, x % y);
}

int calc1024()
{
	int buffer = 1;
	for (int i = 0; i < 10; ++i)
	{
		buffer *= 2;
	}
	return buffer;
}

int fib(int n)
{
	int x;
	if (n < 1)
		return 0;
	if (n == 1)
		return 1;
	return fib(n - 1) + fib(n - 2);
}

int main()
{
	a = 5;
	return a + 2;
}

int kuh(int x)
{
	return x * 2;
}