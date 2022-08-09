CC=gcc
CFLAGS=-g
#CFLAGS=-Wall

all:	interp parse

interp:	interp.c dumpcode.c defs.h
	$(CC) $(CFLAGS) interp.c -o interp

parse: parse.c defs.h
	$(CC) $(CFLAGS) parse.c -o parse

dasm: dumpcode.c defs.h
	$(CC) $(CFLAGS) dumpcode.c -DDISAS -o dasm

parse.oc:	parse.c defs.h
	$(CC) -E -DSELF parse.c | sed -e '/^#/d' -e '/^$$/d' > parse.oc

test: parse interp parse.oc
	$(CC) test/fac.c -o fac
	./fac > fac.out.cc
	./parse < test/fac.c > fac.oo.1
	./interp < fac.oo.1 > fac.out.1
	./parse < parse.oc > parse.oo
	cat parse.oo test/fac.c | ./interp > fac.oo.2
	./interp < fac.oo.2 > fac.out.2
	cmp fac.out.cc fac.out.1
	cmp fac.out.cc fac.out.2
	cat parse.oo parse.oc | ./interp > parse.oo.1
	cmp parse.oo parse.oo.1

clean:
	rm -f parse interp parse.oc *.oo fac *.[12] fac.out.cc test/*.bin

hello: interp parse
	./parse < test/hello.c > test/hello.bin
	./interp < test/hello.bin
