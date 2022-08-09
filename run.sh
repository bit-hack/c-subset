#!/bin/bash
rm -f temp.bin
./parse < $1 > temp.bin
./interp $2 < temp.bin
rm -f temp.bin
