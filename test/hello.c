int
putstr(char *s)
{
  while(*s)
    putchar(*s++);
}

int
main()
{
  putstr("Hello World!\n");
  return 0;
}
