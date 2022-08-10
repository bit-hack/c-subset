

# ----------------------------------------------------------------------------

keywords = [
    'if', 'else', 'return', 'int', 'char', 'do', 'while', 'putchar', 'getchar']

# ----------------------------------------------------------------------------

_seed = 12345

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
        for i in range(0, size):
            out += alpha_num[random(len(alpha_num))]
        if out.lower() not in keywords:
            break
    return out

# ----------------------------------------------------------------------------

class c_global(object):
    def __init__(self, name='bad', type='bad'):
        self.name = name
        self.type = type

    def __repr__(self):
        return '{} {}'.format(self.type, self.name)

class c_local(object):
    def __init__(self, name='bad', type='bad'):
        self.name = name
        self.type = type

    def __repr__(self):
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

# ----------------------------------------------------------------------------

def emit(s):
    sys.stdout.write(s)

def emitln(s):
    sys.stdout.write(s + '\n')

# ----------------------------------------------------------------------------

prog = c_program()

def gen_function(name=None, nargs=None):
    f = c_function(name=name or rand_name())
    prog.funcs += f

    # add arguments
    nargs = nargs or rand_range(0, 4)
    for i in range(0, nargs):
        f.args += c_local()

    # emit declaration
    emitln('int {}({}) {{'.format(f.name, ','.join(f.args)))

    # add locals
    nlocals = rand_range(0, 4)
    for i in range(0, nlocals):
        l = c_local()
        f.locals += l
        emitln('  {};'.format(l))

    # add statements
    # ...

    # terminating brace
    emitln('}')

def gen_global():
    g = c_global(name=rand_name(), type=choose('int', 'char'))
    prog.globals += g

    emitln('{};'.format(g))

def main():
    # generate globals
    nglobals = rand_range(0, 8)
    for i in range(0, nglobals):
        gen_global()
    # generate functions
    nfuncs = rand_range(0, 8)
    for i in range(0, nfuncs):
        gen_function()
    # generate main function
    gen_function(name='main', nargs=0)

if __name__ == '__main__':
    main()
