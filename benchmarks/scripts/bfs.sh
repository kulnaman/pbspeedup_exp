#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J bfs-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
python ./bfs-powerf.py
