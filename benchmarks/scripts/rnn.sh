#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J rnn-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
python ./rnn-powerperf.py  -s 40 -l 2 -hs 512 -b 512 -m 3 -i 400 
