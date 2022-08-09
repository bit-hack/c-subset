#ifndef SELF

#include <stdio.h>

int eputchar(int c) {
  return fputc(c, stderr);
}

#endif

#define T_NAME   256
#define T_CONST  257
#define T_STRING 258

#define RESBASE  512
#define T_RETURN 512
#define T_IF     513
#define T_ELSE   514
#define T_WHILE  515
#define T_DO     516
#define T_INT    517

#define I_CHAR   6

#define RES      7

#include "defs.h"

/* Keep scalars early to get compact addressing modes. */
int curloc;
int lexval;
int token;
int pusharg;
int thechar;
int nglob;
int nlocal;
int nfun;
int strsize;
int narg;
int curgloboffs;
int nsym;
int pushop;

int argids[NARG];
int globids[NGLOB];
int globoffs[NGLOB];
int globscalar[NGLOB];
int funids[NFUN];
int funoffs[NFUN];
int localids[NLOCAL];

char symbol[MAXSYM];
char code[MAXCODE];
char names[MAXNAMES];

#define NAMES                                                                  \
  "return\0if\0else\0while\0do\0int\0char\0getchar\0putchar\0eputchar\0exit"

#define NAMESSIZE 62

/* Output error string */
int eputstr(char *s) {
  while (*s)
    eputchar(*s++);
}

/* Raise a fatal error */
int error(char *s) {
  int i;

  eputstr(s);
  eputstr(" at: ");
  i = 0;
  while (i < 20) {
    eputchar(getchar());
    i++;
  }
  eputstr("\n");
  exit(1);
}

/* Check if character is a digit */
int digit(int c) {
  return '0' <= c && c <= '9';
}

/* Check if a character is alpha-numeric */
int letter(int c) {
  return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_' || digit(c);
}

/* check if strings are equal */
int eqstr(char *p, char *q) {
  while (*p) {
    if (*p++ != *q++)
      return 0;
  }
  return !*q;
}

/* Lookup a name in symbol table */
int lookup(char *name) {
  int i;
  char *ns;

  ns = names;
  i = 0;
  while (i < nsym) {
    if (eqstr(ns, name)) {
      return i;
    }
    while (*ns++)
      ;
    i++;
  }
  while (*ns++ = *name++)
    ;
  return nsym++;
}

/* Advance to next input stream character */
int next() {
  int r;

  r = thechar;
  thechar = getchar();
  return r;
}

int gobble(int t, int rr, int r) {
  if (thechar == t) {
    next();
    return rr;
  }
  return r;
}

/* Parse string up to delimiter */
int getstring(int delim) {
  int c;

  strsize = 0;
  while ((c = next()) != delim) {
    if (c == '\\') {
      if ((c = next()) == 'n')
        c = '\n';
      else if (c == 't')
        c = '\t';
      else if (c == '0')
        c = 0;
    }
    symbol[strsize++] = c;
  }
  symbol[strsize++] = 0;
}

/* Check if 'c' is in string 's' */
int instr(char *s, int c) {
  while (*s) {
    if (*s++ == c)
      return 1;
  }
  return 0;
}

/* Next lexical element */
int getlex() {
  int c;
  char *p;

  /* Skip past all whitespace characters */
  while (0 <= (c = next()) && c <= ' ')
    ;

  /* Braces */
  if (c == -1 || instr("()[]{},;", c)) {
    return c;
  }

  if (c == '/') {
    if (thechar == '*') {
      /* next(); dropping this is wrong */
      while (next() != '*' || thechar != '/')
        ;
      next();
      return getlex();
    } else
      return T_DIV;
  }

  /* Single character tokens */
  if (c == '*') return T_MUL;
  if (c == '%') return T_MOD;
  if (c == '-') return gobble(c, T_POSTDEC, T_SUB);
  if (c == '>') return gobble('=', T_GE, T_GT);
  if (c == '<') return gobble('=', T_LE, T_LT);
  if (c == '=') return gobble(c, T_EQ, T_ASSIGN);
  if (c == '+') return gobble(c, T_POSTINC, T_ADD);
  if (c == '!') return gobble('=', T_NE, T_NOT);
  if (c == '&') return gobble(c, T_ANDAND, T_AND);
  if (c == '|') return gobble(c, T_OROR, T_OR);

  /* Character literal */
  if (c == '\'') {
    getstring(c);
    lexval = symbol[0];
    return T_CONST;
  }

  /* String literal */
  if (c == '"') {
    getstring(c);
    return T_STRING;
  }

  /* Integer literal */
  if (digit(c)) {
    lexval = c - '0';
    while (digit(thechar)) {
      lexval = lexval * 10 + next() - '0';
    }
    return T_CONST;
  }

  /* Name or Identifier */
  if (letter(c)) {
    p = symbol;
    *p++ = c;
    while (letter(thechar))
      *p++ = next();
    *p = 0;
    if ((lexval = lookup(symbol)) < RES) {
      if (lexval == I_CHAR)
        return T_INT;
      return lexval + RESBASE;
    }
    return T_NAME;
  }

  error("Bad input");
}

/* Check if we have specific token */
int istoken(int t) {
  if (token == t) {
    token = getlex();
    return 1;
  }
  return 0;
}

/* Expect a specific token */
int expect(int t) {
  if (!istoken(t)) {
    error("syntax error");
  }
}

/* Parse a type decl */
int type() {
  expect(T_INT);
  while (istoken(T_MUL))
    ;
}

/* Parse a name */
int name() {
  int r;
  if (token != T_NAME)
    error("name expected");
  r = lexval;
  token = getlex();
  return r;
}

/* Emit into code stream */
int emit(int opc) {
  code[curloc++] = opc;
}

/* Emit into specific point in code stream */
int emitat(int a, int c) {
  code[a++] = c;
  code[a] = c / 256;
}

/*Emit instruction and operand */
int emitop(int rator, int rand) {
  int r;

  emit(rator + LITMAX);
  r = curloc;
  emit(rand);
  emit(rand / 256);
  return r;
}

#define emitj emitop

int pushloop(int puop, int max, int *arr) {
  int i;

  i = 0;
  pushop = puop;
  while (i < max) {
    if (arr[i] == lexval) {
      pusharg = i;
      return 1;
    }
    i++;
  }
  return 0;
}

/* Return 1 if lvalue */
int pushval() {
  int lval;

  lval = 1;
  if (pushloop(C_PUSHAL, nlocal, localids)) {
  } else if (pushloop(C_PUSHAA, narg, argids)) {
    pusharg = narg - pusharg - 1;
  } else if (pushloop(C_PUSHAG, nglob, globids)) {
    lval = globscalar[pusharg];
    pusharg = globoffs[pusharg];
  } else {
    lval = 0;
    if (pushloop(C_PUSHAC, nfun, funids)) {
      pusharg = funoffs[pusharg];
    } else if (lexval < RES + NPRECALL) {
      /* predefined function is the last possibility, reuse C_PUSHAC */
      pusharg = lexval - RES;
    } else {
      error("undefined variable");
    }
  }
  emitop(pushop, pusharg);
  return lval;
}

/* Pointer dereference */
int pderef(int l) {
  if (l)
    emit(C_DEREF);
}

/* Parse expression                   */
/* returns true if lvalue, else false */
int expr(int needval, int prec) {
  int na;
  int islval;
  int jdst;
  int op;
  int any;
  int opprec;

  islval = 0;

  /* Parse one expr */
  if (istoken(T_CONST)) {
    emitop(C_PUSHC, lexval);
  } else if (istoken(T_STRING)) {
    /* If the syntax is ok the string must still be in the symbol buffer.
       Stuff it into global space. */
    emitop(C_PUSHS, strsize);
    any = 0;
    while (any < strsize)
      emit(symbol[any++]);
  } else if (istoken(T_NAME)) {
    islval = pushval();
  } else if (istoken('(')) {
    islval = expr(0, P_NONE);
    expect(')');
  } else if (istoken(T_NOT)) {
    expr(1, P_PRE);
    emit(C_NOT);
  } else if (istoken(T_SUB)) {
    expr(1, P_PRE);
    emit(C_NEG);
  } else if (istoken(T_MUL)) {
    expr(1, P_PRE);
    islval = 1;
  } else if (istoken(T_AND)) {
    if (expr(0, P_PRE) == 0)
      error("lvalue required");
  } else
    error("syntax error in expr");

  /* One expression parsed, try for hi precidence ops */
  any = 1;
  while (any) {
    op = token % OPMOD;
    if (istoken('(')) {
      /* function call */
      pderef(islval);
      na = 0;
      if (!istoken(')')) {
        do {
          expr(1, P_NONE);
          na++;
        } while (istoken(','));
        expect(')');
      }
      emitop(C_CALL, na * 2);
      islval = 0;
    } else if (istoken('[')) {
      /* array ref */
      pderef(islval);
      expr(1, P_NONE);
      emit(C_ADD);
      expect(']');
      islval = 1;
    } else if (istoken(T_POSTINC) || istoken(T_POSTDEC)) {
      if (!islval)
        error("no lval for ++");
      emit(op);
      islval = 0;
    } else
      any = 0;
  }

  opprec = token / OPMOD;
  while (prec < opprec) {
    if ((op = token % OPMOD) != C_ASSIGN) {
      pderef(islval);
    } else {
      if (!islval)
        error("no lval for =");
    }
    if (istoken(T_ANDAND) || istoken(T_OROR)) {
      emit(C_DUP);
      if (op == C_OROR)
        emit(C_NOT);
      jdst = emitj(C_JFALSE, 0);
      emit(C_POP);
      expr(1, opprec);
      emitat(jdst, curloc);
    } else {
      /* emit binop code */
      token = getlex();
      expr(1, opprec);
      emit(op);
    }
    islval = 0;
    opprec = token / OPMOD;
  }
  if (needval) {
    pderef(islval);
    islval = 0;
  }
  return islval;
}

/* Parse expression in parenthesis */
int pexpr() {
  expect('(');
  expr(1, P_NONE);
  expect(')');
}

/* Parse statement */
int stmt() {
  int jdest;
  int tst;

  if (istoken('{')) {
    while (!istoken('}'))
      stmt();
  } else if (istoken(T_IF)) {
    pexpr();
    jdest = emitj(C_JFALSE, 0);
    stmt();
    if (istoken(T_ELSE)) {
      tst = emitj(C_JUMP, 0);
      emitat(jdest, curloc);
      stmt();
      emitat(tst, curloc);
    } else {
      emitat(jdest, curloc);
    }
  } else if (istoken(T_WHILE)) {
    tst = curloc;
    pexpr();
    jdest = emitj(C_JFALSE, 0);
    stmt();
    emitj(C_JUMP, tst);
    emitat(jdest, curloc);
  } else if (istoken(T_DO)) {
    jdest = curloc;
    stmt();
    expect(T_WHILE);
    pexpr();
    emit(C_NOT);
    emitj(C_JFALSE, jdest);
  } else if (istoken(T_RETURN)) {
    expr(1, P_NONE);
    expect(';');
    emit(C_RETURN);
  } else if (istoken(';')) {
    /* empty */
  } else {
    /* just an expression */
    expr(1, P_NONE);
    emit(C_POP);
    expect(';');
  }
}

int parseFunc(int objid) {
  /* Add function */
  funids[nfun] = objid;
  funoffs[nfun++] = curloc;
  
  /* Parse arguments */
  narg = 0;
  if (!istoken(')')) {
    do {
      type();
      argids[narg++] = name();
    } while (istoken(','));
    expect(')');
  }

  /* Start of function body */
  expect('{');

  /* Local var decls */
  nlocal = 0;
  while (token == T_INT) {
    type();
    do {
      localids[nlocal++] = name();
    } while (istoken(','));
    expect(';');
  }

  /* Allocate stack space for local vars */
  if (DEFSTK < nlocal)
    emitop(C_ALLOC, nlocal);

  /* Statements until end of function body */
  while (!istoken('}'))
    stmt();

  emit(C_RETURN);
  
  return 1;
}

int parseGlobalVar(int objid) {
  /* Global variable */
  globoffs[nglob] = curgloboffs;
  
  /* If array declaration */
  if (istoken('[')) {
    expect(T_CONST);
    curgloboffs = curgloboffs + lexval;
    expect(']');
    globscalar[nglob] = 0;
  } else {
    curgloboffs++;
    globscalar[nglob] = 1;
  }
  globids[nglob++] = objid;
  expect(';');
  
  return 1;
}

int parse() {
  int objid;

  token = getlex();
  
  /* While EOF not encountered */
  while (token >= 0) {

    /* Type declaration */
    type();
    objid = name();

    /* If is function declaration */
    if (istoken('(')) {
      parseFunc(objid);

    } else {
      parseGlobalVar(objid);
    }
  }

  return 1;
}

int main() {
  int n;
  char *p;
  char *q;

  /* Initialize symbol table */
  nsym = RES + NPRECALL;
  p = names;
  q = NAMES;
  n = NAMESSIZE;
  do
    *p++ = *q++;
  while (n--);

  /* Parse the input file */
  curloc = 10; /* Some space to avoid low addrs */
  thechar = getchar();
  parse();
  n = curloc;

  /* Generate code to jump to main at start of code */
  curloc = 0;
  lexval = lookup("main");
  pushval();
  emitop(C_CALL, 0);
  emit(C_EXIT);

  /* Number of names */
  putchar(n);
  putchar(n / 256);

  /* Emit all code to stdout */
  p = code;
  while (n--) {
    putchar(*p++);
  }

  return 0;
}
