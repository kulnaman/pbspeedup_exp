#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J cufft-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00

python ./cufft-powerperf.py
