#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J dgemm-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
python ./dgemm-powperf.py -p `expr 4096 \* 4`

