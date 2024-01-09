#!/bin/bash -xe
#SBATCH -N 1
#SBATCH -w deepgreen
#SBATCH -J cublas-cpu-gpu
#SBATCH --exclusive
#SBATCH -t 24:00:00
echo "starting 8192"
apps=('bt' 'cg' 'ep' 'ft' 'lu' 'mg' 'is' 'sp' 'ua')
problem_sizes=('C')

for i in "${apps[@]}"
do 

    ./npb-powerperf.py -p `expr 4096 \* 2`
done
#echo "starting `expr 4096 \* 3`"
#./dgemm-powperf.py -p `expr 4096 \* 3`

#echo "starting `expr 4096 \* 4`"
#./dgemm-powperf.py -p `expr 4096 \* 4`
