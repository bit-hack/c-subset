CC=gcc
CFLAGS=-g
#CFLAGS=-Wall

all:	interp parse

interp:	interp.c dumpcode.c defs.h
	$(CC) $(CFLAGS) interp.c -o interp

parse: parse.c defs.h
	$(CC) $(CFLAGS) parse.c -o parse

parse.oc:	parse.c defs.h
	$(CC) -E -DSELF parse.c | sed -e '/^#/d' -e '/^$$/d' > parse.oc

test: parse interp parse.oc
	$(CC) fac.c -o fac
	./fac.exe > fac.out.cc
	./parse.exe < fac.c > fac.oo.1
	./interp.exe < fac.oo.1 > fac.out.1
	./parse.exe < parse.oc > parse.oo
	cat parse.oo fac.c | ./interp.exe > fac.oo.2
	./interp.exe < fac.oo.2 > fac.out.2
	cmp fac.out.cc fac.out.1
	cmp fac.out.cc fac.out.2
	cat parse.oo parse.oc | ./interp.exe > parse.oo.1
	cmp parse.oo parse.oo.1

clean:
	rm -f parse interp parse.oc *.oo fac *.[12] fac.out.cc
