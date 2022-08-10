

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
    def __init__(self):
        self.name = rand_name()
        self.type = choose(['int', 'char'])

class c_local(object):
    def __init__(self):
        self.name = rand_name()
        self.type = choose(['int', 'char'])

class c_function(object):
    def __init__(self):
        self.name   = 'bad'
        self.args   = []
        self.locals = []

    def generate(self):
        self.name = rand_name()

        nargs = rand_range(0, 4)
        for i in range(0, nargs):
            self.args += c_local() 

        nlocals = rand_range(0, 4)
        for i in range(0, nlocals):
            self.locals += c_local() 

class c_program(object):
    def __init__(self):
        self.globals = []
        self.funcs = []
    
    def generate(self):
        nglobals = rand_range(0, 8)
        for i in range(0, nglobals):
            self.globals += c_global()
        
        nfuncs = rand_range(0, 4)
        for i in range(0, nfuncs):
            self.funcs += c_function()

# ----------------------------------------------------------------------------

def gen_global():
    pass

def gen_function():
    pass

def main():
    pass

if __name__ == '__main__':
    main()
