#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J cublas-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
echo "starting 8192"
python ./cublas-powerperf.py -p `expr 4096 \* 4`

