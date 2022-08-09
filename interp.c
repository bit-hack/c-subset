#include <stdio.h>
#include <string.h>

int eputchar(int c) {
  return fputc(c, stderr);
}

#include "defs.h"

#define MAXSTACK 10000
#define MAXMEM (MAXCODE + MAXDATA + MAXSTACK)
int mem[MAXMEM];

int verbosef;

/* Top of stack */
#define TOS mem[sp]

/* Pop from stack */
#define POP mem[sp++]

/* Push onto stack */
#define PUSH(x)                                                                \
  sp--;                                                                        \
  TOS = x

#define CASE(l, s)                                                             \
  case l:                                                                      \
    s;                                                                         \
    break;

/* Binary operator */
#define OP2(l, o) CASE(l, r = POP; TOS = TOS o r)

/* Unary operator */
#define OP1(l, o) CASE(l, TOS = o TOS)

/* Push  */
#define OPPUSH(l, s) CASE(l, PUSH(s))

/* Literal */
#define LIT mem[pc++]

/* Return from function */
#define DORET                                                                  \
  sp = sp + lit / 2;                                                           \
  TOS = r;                                                                     \
  if (lit % 2)                                                                 \
    sp++

#define code mem
#include "dumpcode.c"

int main(int argc, char **argv) {
  int *q, sp, fp, r, opc, lit, pc, codesize;
  int oldpc;

  verbosef = argc > 1;

  /* Read in code size */
  codesize = getchar();
  codesize = codesize + getchar() * 256;

  pc = codesize;
  q = mem;

  /* Read the code from stdin */
  while (pc--) {
    *q++ = getchar();
  }

  if (verbosef)
    fprintf(stderr, "code size %d\n", codesize);
  sp = MAXMEM;
  pc = 0;

  /* Start execution */
  while (1) {

    if (verbosef) {
      fprintf(stderr, "%5d %5d(%5d,%5d,%5d) ", fp, sp, mem[sp], mem[sp + 1],
              mem[sp + 2]);
      dumpcode(pc, pc + 1);
    }

    oldpc = pc;

    switch (opc = LIT) {
      OP1(C_NOT, !)
      OP1(C_NEG, -)
      OP2(C_ADD, +)
      OP2(C_SUB, -)
      OP2(C_MUL, *)
      OP2(C_DIV, /)
      OP2(C_MOD, %)
      OP2(C_AND, &)
      OP2(C_OR,  |)
      OP2(C_GT,  >)
      OP2(C_GE,  >=)
      OP2(C_LE,  <=)
      OP2(C_EQ,  ==)
      OP2(C_NE,  !=)
      OP2(C_LT,  <)
      CASE(C_ASSIGN,    r = POP;
                        mem[TOS] = r;
                        TOS = r)
      CASE(C_ASSIGNPOP, r = POP;
                        mem[TOS] = r;
                        sp++)
      CASE(C_RETURN,    r   = TOS;
                        sp  = fp;
                        pc  = mem[sp - F_PC];
                        fp  = mem[sp - F_FP];
                        lit = mem[sp - F_NA];
                        DORET)
      CASE(C_POP,       sp++)
      CASE(C_EXIT,      return TOS)
      CASE(C_DUP,       r = TOS;
                        PUSH(r))
      CASE(C_DEREF,     TOS = mem[TOS])
      CASE(C_POSTINC,   TOS = mem[TOS]++)
      CASE(C_POSTDEC,   TOS = mem[TOS]--)

      /* Try the combined opcodes/literals */
      default: {

        lit = opc % LITMOD;
        opc = opc / LITMOD;

        if (lit == LITMAX) {
          lit = mem[pc++];
          lit = lit + mem[pc++] * 256;
        } else if (LITMAX < lit) {
          lit = lit + mem[pc++] * LITMUL - 1;
        }

        switch (opc) {
          OPPUSH(X_PUSHAA, fp + lit)
          OPPUSH(X_PUSHAL, fp - lit - FRAMESIZE - 1)
          OPPUSH(X_PUSHA,  mem[fp + lit])
          OPPUSH(X_PUSHL,  mem[fp - lit - FRAMESIZE - 1])
          OPPUSH(X_PUSHAG, codesize + lit)
          OPPUSH(X_PUSHG,  mem[codesize + lit])
          OPPUSH(X_PUSHAC, lit)
          OPPUSH(X_PUSHC,  lit)
          CASE(X_PUSHS, PUSH(pc); pc = pc + lit)
          CASE(X_ALLOC, sp = sp - lit)
          CASE(X_CALL, 
              r = mem[sp - -lit / 2];
              if (r < NPRECALL + 1) {
                switch (r) {
                case 0: r = getchar();     break;
                case 1: r = putchar(TOS);  break;
                case 2: r = eputchar(TOS); break;
                case 3:     exit(TOS);     break;
                }
                DORET;
              } else {
                mem[sp - F_PC] = pc;
                mem[sp - F_FP] = fp;
                mem[sp - F_NA] = lit;
                fp = sp;
                sp = sp - FRAMESIZE - DEFSTK;
                pc = r;
              })
          CASE(X_JUMP,             pc = lit)
          CASE(X_TRUE,   if ( POP) pc = lit)
          CASE(X_JFALSE, if (!POP) pc = lit)
          default:
            fprintf(stderr, "unimpl op %d at %d\n", mem[oldpc], oldpc);
            exit(1);
        }  // switch
      }
    }  // switch
  }  // while
}
