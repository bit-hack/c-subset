int putstr(char *s) {
  while (*s)
    putchar(*s++);
}

int putn(int n) {
  if (n >= 10)
    putn(n / 10);
  putchar((n % 10) + '0');
}

int fib(int range)
{
  int a;
  int b;
  int c;

  a = 0;
  b = 1;

  while (a <= range) {
    putn(a);
    putstr("\n");
    c = a+b;
    a = b;
    b = c;
  }
}

int main()
{
  int range;

  range = 50;
  fib(range);

  return 0;
}
