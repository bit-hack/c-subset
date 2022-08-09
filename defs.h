#define NARG        50  /* max arguments                        */
#define NLOCAL      50  /* max locals                           */
#define NGLOB       300 /* max globals                          */
#define NFUN        300 /* max functions                        */

#define MAXNAMES  30000
#define MAXCODE   10000
#define MAXDATA   30000
#define MAXSYM    30000

#define OPMOD       100 /* token and precidence combiner        */

#define LITMOD      16  /* opcode/lit combiner                  */
#define LITMAX      7   /* max combined opcode/literal value    */
                        /* special case outwith this value      */
#define LITMUL      8
#define LITLIM      32  /* start of opcode/lit (C_PUSHAA)       */

/* combined opcodes where lower 4 bits are encoded as a literal */
#define X_PUSHAA    2   /* push arg addr:   fp+lit              */
#define X_PUSHA     3   /* push arg:       [fp+lit]             */
#define X_PUSHAL    4   /* push local addr: fp-lit-framesize-1  */
#define X_PUSHL     5   /* push local:     [fp-lit-framesize-1] */
#define X_PUSHAG    6   /* push global addr: codesize + lit     */
#define X_PUSHG     7   /* push global:     [codesize + lit]    */
#define X_PUSHC     8   /* push constant                        */
#define X_PUSHS     9   /* push string                          */
#define X_ALLOC     10  /* stack allocation                     */
#define X_CALL      11  /* call subroutine                      */
#define X_PUSHAC    12  /* push address code                    */
#define X_JUMP      13  /* unconditional jump                   */
#define X_JFALSE    14  /* jump if false                        */
#define X_TRUE      15  /* jump if true                         */

/* high forms of the combined opcodes                           */
#define C_PUSHAA    (LITMOD * X_PUSHAA)
#define C_PUSHA     (LITMOD * X_PUSHA )
#define C_PUSHAL    (LITMOD * X_PUSHAL)
#define C_PUSHL     (LITMOD * X_PUSHL )
#define C_PUSHAG    (LITMOD * X_PUSHAG)
#define C_PUSHG     (LITMOD * X_PUSHG )
#define C_PUSHC     (LITMOD * X_PUSHC )
#define C_PUSHS     (LITMOD * X_PUSHS )
#define C_ALLOC     (LITMOD * X_ALLOC )
#define C_CALL      (LITMOD * X_CALL  )
#define C_PUSHAC    (LITMOD * X_PUSHAC)
#define C_JUMP      (LITMOD * X_JUMP  )
#define C_JFALSE    (LITMOD * X_JFALSE)
#define C_TRUE      (LITMOD * X_TRUE  )

#define C_ASSIGN    1
#define C_OROR      2   /* logical or                           */
#define C_ANDAND    3   /* logical and                          */
#define C_OR        4   /* bitwise or                           */
#define C_AND       5   /* bitwise and                          */
#define C_EQ        6   /* equal                                */
#define C_NE        7   /* not equal                            */
#define C_LT        8   /* less than                            */
#define C_ASSIGNPOP 9   /* assign and pop (drop)  (frequent)    */
#define C_RETURN    10  /* return from subroutine (frequent)    */
#define C_GE        11  /* greater than or equal                */
#define C_ADD       12  /* add                                  */
#define C_SUB       13  /* subtract                             */
#define C_MUL       14  /* multiply                             */
#define C_DIV       15  /* divide                               */
#define C_MOD       16  /* modulo                               */
#define C_NOT       17  /* unary not                            */
#define C_NEG       18  /* unary negate                         */
#define C_POSTINC   19  /* dereference with post increment      */
#define C_DUP       21  /* duplicate top of stack               */
#define C_POP       22  /* pop (drop) top of stack              */
#define C_DEREF     23  /* dereference                          */
#define C_LE        24  /* less than or equal to                */
#define C_POSTDEC   25  /* dereference with post decrement      */
#define C_GT        26  /* greater than                         */
#define C_EXIT      27  /* builtin exit()                       */

/* Tokens                                                       */
#define TOKEN(PREC, OP) ((PREC * OPMOD) + OP)
#define T_ASSIGN    TOKEN( 1,  1)
#define T_OROR      TOKEN( 3,  2)
#define T_ANDAND    TOKEN( 4,  3)
#define T_OR        TOKEN( 5,  4)
#define T_AND       TOKEN( 7,  5)
#define T_EQ        TOKEN( 8,  6)
#define T_NE        TOKEN( 8,  7)
#define T_LT        TOKEN( 8,  8)
#define T_GE        TOKEN( 8, 11)
#define T_LE        TOKEN( 8, 24)
#define T_GT        TOKEN( 8, 26)
#define T_ADD       TOKEN(10, 12)
#define T_SUB       TOKEN(10, 13)
#define T_MUL       TOKEN(11, 14)
#define T_DIV       TOKEN(11, 15)
#define T_MOD       TOKEN(11, 16)
#define T_NOT       TOKEN(12, 17)
#define T_NEG       TOKEN(12, 18)
#define T_POSTINC   TOKEN(12, 19)
#define T_INV       TOKEN(12, 20)
#define T_POSTDEC   TOKEN(12, 25)

#define P_NONE      0   /* No precedence                        */
#define P_PRE       100 /* ? */

/* operations with no operands */

#define FRAMESIZE   3   /* Frame book keeping size              */
#define F_PC        1   /* Frame PC (return address)            */
#define F_FP        2   /* Frame FP (old frame pointer)         */
#define F_NA        3   /* Frame number of arguments            */

#define DEFSTK      16  /* Default stack?                       */

#define NPRECALL    4   /* Related to builtin calls?            */
