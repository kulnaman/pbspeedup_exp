#!/bin/bash

module load cuda
make clean
make

size=`expr 4096 \* 4`
nvprof --metrics flop_count_sp ./matrixMul2 -wA=$size -hA=$size -wB=$size -hB=$size
