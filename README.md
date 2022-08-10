# Compiler for a small subset of C

This repository contains a resurrected version of Lennart Augustssons compiler for a small subset of C.

My goal with this project is to clean up and document the code to make it a good learning vehicle.

Do `make test` to compile and test everything.

`parse.c` contains a compiler that emits byte code for the C code it reads on stdin.

`interp.c` contains the interpreter for the byte code.

`parse.c` is written in the same subset of C that it can compile.  interp.c is not, but with some minimal tweaking it can be.
