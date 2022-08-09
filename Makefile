CC=gcc
CFLAGS=-g
#CFLAGS=-Wall

all:	interp.exe parse.exe

interp.exe:	interp.c dumpcode.c defs.h
	$(CC) $(CFLAGS) interp.c -o interp

parse.exe: parse.c defs.h
	$(CC) $(CFLAGS) parse.c -o parse

parse.oc:	parse.c defs.h
	$(CC) -E -DSELF parse.c | sed -e '/^#/d' -e '/^$$/d' > parse.oc

test: parse.exe interp.exe parse.oc
	$(CC) test/fac.c -o fac
	./fac.exe > fac.out.cc
	./parse.exe < test/fac.c > fac.oo.1
	./interp.exe < fac.oo.1 > fac.out.1
	./parse.exe < parse.oc > parse.oo
	cat parse.oo test/fac.c | ./interp.exe > fac.oo.2
	./interp.exe < fac.oo.2 > fac.out.2
	cmp fac.out.cc fac.out.1
	cmp fac.out.cc fac.out.2
	cat parse.oo parse.oc | ./interp.exe > parse.oo.1
	cmp parse.oo parse.oo.1

clean:
	rm -f parse.exe interp.exe parse.oc *.oo fac *.[12] fac.out.cc test/*.bin

hello: interp.exe parse.exe
	./parse.exe < test/hello.c > test/hello.bin
	./interp.exe < test/hello.bin
