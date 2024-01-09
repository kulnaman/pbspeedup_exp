#!/bin/bash

sm=1911
mem=5705
smo=0
memo=0
size=`expr 4096 \* 2`
if [ "$#" -ne 5 ]; then
    echo "Using default sm/mem values because nargs != 4"
else
    mem=$1
    sm=$2
    memo=$3
    smo=$4
    size=$5
fi

make
module load cuda
#size=4096

pmon="../../tools/gpu_power/monitor"

nvidia-settings -a "GPUFanControlState=1"
nvidia-settings -a "[fan-0]/GPUTargetFanSpeed=100"
sleep 2

echo $sm > power.csv
echo $mem >> power.csv
echo $smo >> power.csv
echo $memo >> power.csv
echo "cublas-${size}" >> power.csv
echo "CUBLAS ${size}x${size}" >> power.csv
$pmon >> power.csv &
pid=$!
time ./matrixMul2 -wA=$size -hA=$size -wB=$size -hB=$size
kill -s SIGINT $pid
nvidia-settings -a "GPUFanControlState=0"
