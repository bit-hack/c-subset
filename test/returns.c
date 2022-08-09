int putstr(char *s) {
  while (*s)
    putchar(*s++);
}

int putn(int n) {
  if (n >= 10)
    putn(n / 10);
  putchar((n % 10) + '0');
}

int arg0() {
  return 1;
}

int arg1() {
  int a;
  a = 2;
  return 2;
}

int arg2(int a) {
  return a;
}

int arg3(int a, int b) {
  return a + b;
}

int arg4(int a) {
  int b;
  b = 3;
  return a + b;
}

int main() {

  putn( arg0() );
  putstr("\n");

  putn( arg1() );
  putstr("\n");

  putn( arg2(3) );
  putstr("\n");

  putn( arg3(3, 1) );
  putstr("\n");

  putn( arg4(2) );
  putstr("\n");

  return 0;
}
