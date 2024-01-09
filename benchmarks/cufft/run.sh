#!/bin/bash


export DISPLAY=:0
sm=1911
mem=5705
smo=0
memo=0
size=80000000
if [ "$#" -ne 5 ]; then
    echo "Using default sm/mem values because nargs != 5"
else
    mem=$1
    sm=$2
    memo=$3
    smo=$4
    size=$5
fi

module load cuda
osize=$size

pmon="../../tools/gpu_power/monitor"

nvidia-settings -a "GPUFanControlState=1"
nvidia-settings -a "[fan-0]/GPUTargetFanSpeed=100"
sleep 2

echo $sm > power.csv
echo $mem >> power.csv
echo $smo >> power.csv
echo $memo >> power.csv
echo "cufft-${osize}" >> power.csv
echo "cufft" >> power.csv
$pmon >> power.csv &
pid=$!
time ./simpleCUFFT
kill -s SIGINT $pid
nvidia-settings -a "GPUFanControlState=0"
