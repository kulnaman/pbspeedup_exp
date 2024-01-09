#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J minife-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
#python ./miniFE-powperf.py -nx 400 -ny 200 -nz 100
python ./miniFE-powperf.py -nx 600 -ny 300 -nz 150
