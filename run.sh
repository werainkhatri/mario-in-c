#!/bin/bash
file="moving-car"
gcc -o $file $file.c -lgraph
./$file
rm $file