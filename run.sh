#!/bin/bash
rm -f temp.bin
./parse.exe < $1 > temp.bin
./interp.exe < temp.bin
rm -f temp.bin
