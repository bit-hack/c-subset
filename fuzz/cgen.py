#!/usr/bin/python

from ast import stmt
import sys

# ----------------------------------------------------------------------------

keywords = [
    'if', 'else', 'return', 'int', 'char', 'do', 'while', 'putchar', 'getchar']

# ----------------------------------------------------------------------------

_seed = 12348

def _random():
    global _seed
    x = _seed
    x ^= x >> 12 & 0xffffffff
    x ^= x << 25 & 0xffffffff
    x ^= x >> 27 & 0xffffffff
    _seed = x    & 0xffffffff
    return (x * 0x4F6CDD1D) >> 8

def random(maximum):
    return _random() % maximum if maximum else 0

def choose(*args):
    return args[random(len(args))]

def chance(percent):
    return random(100) < percent

def rand_range(lo, hi):
    return lo + random(hi-lo)

def rand_name():
    alpha_num = '_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLNMOPQRSTUVWXYZ0123456789'
    while True:
        size = rand_range(1, 8)
        out = alpha_num[random(26 * 2 + 1)]
        for _ in range(0, size):
            out += alpha_num[random(len(alpha_num))]
        if out.lower() not in keywords:
            break
    return out

def rand_type():
    return choose('int', 'char')

# ----------------------------------------------------------------------------

class BacktrackError(Exception):
    pass

class c_var(object):
    def __init__(self, name='bad', type='bad'):
        self.name = name
        self.type = type

    def __str__(self):
        return '{} {}'.format(self.type, self.name)

class c_function(object):
    def __init__(self, name='bad'):
        self.name   = name
        self.args   = []
        self.locals = []

class c_program(object):
    def __init__(self):
        self.globals = []
        self.funcs   = []
    
    def top_func(self):
        return self.funcs[-1]

    def rand_ident(self, locals=True, args=True, globals=True):
        f = self.top_func()
        l = []
        if locals and f.locals:
            l = l + [l for l in f.locals]
        if args and f.args:
            l = l + [a for a in f.args]
        if globals and self.globals:
            l = l + [g for g in self.globals]
        return choose(*l) if l else None

    def rand_func(self):
        return choose(*self.funcs)

# ----------------------------------------------------------------------------

_indent = 1

def indent():
    emit('  ' * _indent)

def indent_inc():
    global _indent
    _indent = _indent + 1

def indent_dec():
    global _indent
    _indent = _indent - 1

def emit(s):
    sys.stdout.write(s)

def emitln(s):
    sys.stdout.write(s + '\n')

# ----------------------------------------------------------------------------

prog = c_program()

_stmt_complex = 0
_expr_complex = 0

def gen_expr_bin():
    ops = ['+', '-', '/', '*', '||', '&&', '%']
    op = choose(*ops)
    gen_expr(); emit(' {} '.format(op)); gen_expr()

def gen_expr_unary():
    ops = [' !', ' -']
    op = choose(*ops)
    emit('{}'.format(op)); gen_expr();

def gen_expr_paren():
    emit('('); gen_expr(); emit(')')

def gen_expr_ident():
    ident = prog.rand_ident()
    if not ident:
        raise BacktrackError
    emit(ident.name)

def gen_expr_const():
    emit(str(rand_range(0, 255)))

def gen_expr_char():
    emit('\'{}\''.format(chr(rand_range(ord('a'), ord('z')))))

def gen_expr_assign():
    ident = prog.rand_ident()
    if not ident:
        raise BacktrackError
    emit('('); emit(ident.name); emit(' = '); gen_expr(); emit(')')

def gen_expr_call():
    f = prog.rand_func()
    emit('{}('.format(f.name))
    for i, _ in enumerate(f.args):
        emit(', ' if i else ''); gen_expr()
    emit(')')

def gen_expr():
    global _expr_complex
    funcs = [
        gen_expr_const,
        gen_expr_char,
        gen_expr_ident,
        gen_expr_paren  if _expr_complex <= 4 else None,
        gen_expr_unary  if _expr_complex <= 4 else None,
        gen_expr_bin    if _expr_complex <= 3 else None,
        gen_expr_assign if _expr_complex <= 2 else None,
        gen_expr_call   if _expr_complex <= 2 else None
    ]
    while True:
        try:
            f = choose(*funcs)
            if f:
                _expr_complex = _expr_complex + 1
                f()
                _expr_complex = _expr_complex - 1
                return
        except BacktrackError:
            pass

def gen_stmt_putchar():
    indent(); emit('putchar('); gen_expr_char(); emitln(');')

def gen_stmt_return():
    indent(); emit('return '); gen_expr(); emitln(';')

def gen_stmt_if():
    indent(); emit('if ('); gen_expr(); emitln(')')
    indent_inc()
    gen_stmt()
    indent_dec()

def gen_stmt_compound():
    indent(); emitln('{')
    indent_inc()
    gen_stmt_multi()
    indent_dec()
    indent(); emitln('}')

def gen_stmt_expr():
    indent(); gen_expr(); emitln(';')

def gen_stmt_while():
    indent(); emit('while ('); gen_expr(); emitln(')')
    indent_inc()
    gen_stmt()
    indent_dec()

def gen_stmt_do():
    indent(); emitln('do {')
    indent_inc()
    gen_stmt()
    indent_dec()
    indent(); emit('} while ('); gen_expr(); emitln(');')

def gen_stmt_multi():
    nstmt = rand_range(0, 4)
    for _ in range(nstmt):
        gen_stmt()

def gen_stmt():
    global _stmt_complex
    funcs = [
        gen_stmt_return   if _stmt_complex >= 1 else None,
        gen_stmt_putchar ,
        gen_stmt_putchar ,
        gen_stmt_putchar ,
        gen_stmt_if       if _stmt_complex <= 3 else None,
        gen_stmt_compound if _stmt_complex <= 2 else None,
        gen_stmt_compound if _stmt_complex <= 2 else None,
        gen_stmt_expr     if _stmt_complex <= 4 else None,
        gen_stmt_expr     if _stmt_complex <= 4 else None,
        gen_stmt_while    if _stmt_complex <= 3 else None,
        gen_stmt_do       if _stmt_complex <= 3 else None
    ]
    while True:
        f = choose(*funcs)
        if f:
            _stmt_complex = _stmt_complex + 1
            f()
            _stmt_complex = _stmt_complex - 1
            return

def gen_function(name=None, nargs=None):
    f = c_function(name=name or rand_name())
    prog.funcs += [f]

    # add arguments
    nargs = nargs if nargs is not None else rand_range(0, 4)
    for _ in range(0, nargs):
        f.args += [c_var(name=rand_name(), type=rand_type())]

    # emit declaration
    arg_decls = ', '.join([str(e) for e in f.args])
    emitln('int {}({}) {{'.format(f.name, arg_decls))

    # add locals
    nlocals = rand_range(0, 4)
    for _ in range(0, nlocals):
        l = c_var(name=rand_name(), type=rand_type())
        f.locals += [l]
        emitln('  {};'.format(l))

    # add statements
    nstmts = rand_range(0, 7)
    for _ in range(0, nstmts):
        gen_stmt()

    # terminating brace
    emitln('}')

def gen_global():
    g = c_var(name=rand_name(), type=rand_type())
    prog.globals += [g]
    emitln('{};'.format(g))

def main():
    # generate globals
    nglobals = rand_range(0, 8)
    for _ in range(0, nglobals):
        gen_global()
    # generate functions
    nfuncs = rand_range(0, 7)
    for _ in range(0, nfuncs):
        gen_function()
    # generate main function
    gen_function(name='main', nargs=0)

if __name__ == '__main__':
    _seed = int(sys.argv[1]) if len(sys.argv) > 1 else 12345
    emitln('/* seed={} */'.format(_seed))
    main()
