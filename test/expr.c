int g;

int putstr(char *s) {
  while (*s)
    putchar(*s++);
}

int putn(int n) {
  if (n >= 10)
    putn(n / 10);
  putchar((n % 10) + '0');
}

int putval(int n) {
  putn(n);
  putstr("\n");
}

int test0_() {
  return g = g + 1;
}

int test0() {
  int g_;

  g  = 10;
  g_ = test0();
  putval(g);
  putval(g_);
}

int test1() {

  putval(2 || 4);  /* 1  */
  putval(2 && 4);  /* 1  */
  
  putval(2 | 4);   /* 6  */
  putval(2 & 4);   /* 0  */
  
  putval(3 + 5);   /* 8  */
  putval(5 - 3);   /* 2  */
  
  putval(3 * 5);   /* 15 */
  putval(8 / 2);   /* 4  */
}

int main() {

  /* unimpl op 0 at 143 */
  test0();
  test1();

  return 0;
}
