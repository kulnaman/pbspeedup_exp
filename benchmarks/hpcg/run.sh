#!/bin/bash
#location of HPL 
HPCG_DIR=`pwd`

export XAUTHORITY=/var/run/lightdm/root/:0 && export DISPLAY=:0 && sudo xhost +
export DISPLAY=:0
#*************************************************************************************************
# env modules below ere tested on nvidia psg cluster 
#**************************************************************************************************
sm=1911
mem=5705
smo=0
memo=0
nx=128
ny=128
nz=128
if [ "$#" -ne 7 ]; then
    echo "Using default sm/mem values because nargs != 7"
else
    mem=$1
    sm=$2
    memo=$3
    smo=$4
    nx=$5
    ny=$6
    nz=$7
fi


echo $sm > power.csv
echo $mem >> power.csv
echo $smo >> power.csv
echo $memo >> power.csv
echo "hpcg-${nx}x${ny}x${nz}" >> power.csv
echo "hpcg ${nx}x${ny}x${nz}" >> power.csv

pmon="../../tools/gpu_power/monitor"

nvidia-settings -a "GPUFanControlState=1"
nvidia-settings -a "[fan-0]/GPUTargetFanSpeed=100"
sleep 2

HPCG_BIN=xhpcg-3.1_gcc_485_cuda-10.0.130_ompi-3.1.0_sm_35_sm_50_sm_60_sm_70_sm_75_ver_10_9_18

cp hpcg.dat_128x128x128_60 hpcg.dat
export CUDA_VISIBLE_DEVICES=0
MPI=1
CORES=24

module load mpi/openmpi-3.1.6
export LD_RUN_PATH=$LD_RUN_PATH:/usr/local/cuda-10.0/lib64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda-10.0/lib64
export OMP_NUM_THREADS=24
export OMP_PLACES=cores
export OMP_PROC_BIND=true

rm -f *.yaml *.txt
$pmon >> power.csv &
pid=$!
mpirun -np 1 ./$HPCG_BIN
kill -s SIGINT $pid
nvidia-settings -a "GPUFanControlState=0"
sleep 5
sync
echo "FOOTER" >> power.csv
grep "HPCG result is VALID with a GFLOP/s rating of:" *.yaml | tr -d ' ' | awk -F: '{print $2}' >> power.csv

