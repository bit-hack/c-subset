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
  putn(0);
  putstr("\n");
  return 1;
}

int arg1(int a) {
  putn(a);
  putstr("\n");
  return 1;
}

int arg2(int a, int b) {
  putn(a);
  putstr("\n");
  arg1(b);
  return 1;
}

int arg3(int a, int b, int c) {
  putn(a);
  putstr("\n");
  arg2(b, c);
  return 1;
}

int arg4(int a, int b, int c, int d) {
  putn(a);
  putstr("\n");
  arg3(b, c, d);
  return 1;
}

int arg5(int a, int b, int c, int d, int e) {
  putn(a);
  putstr("\n");
  arg4(b, c, d, e);
  return 1;
}

int main() {

  putstr("--------\n");
  arg0();

  putstr("--------\n");
  arg1(1);

  putstr("--------\n");
  arg2(1, 2);

  putstr("--------\n");
  arg3(1, 2, 3);

  putstr("--------\n");
  arg4(1, 2, 3, 4);

  putstr("--------\n");
  arg5(1, 2, 3, 4, 5);

  return 0;
}
