#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J conv-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
python ./conv-powerf.py -iter 2000
