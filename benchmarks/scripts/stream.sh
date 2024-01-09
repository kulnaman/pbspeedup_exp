#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J stream-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
python ./stream-powerperf.py -p `expr 402653184 \* 2` 
#./dgemm-powperf.py -p `expr 4096 \* 3`

#echo "starting `expr 4096 \* 4`"
#./dgemm-powperf.py -p `expr 4096 \* 4`

