#!/bin/bash

# Benchmark infomation
basedir=/home/rge/tpds2020
raplcmd=/usr/local/bin/rapl_power_meter
powerlimitcmd=$basedir/utils/mu_power_gadget
benchmark_dir=$basedir/benchmark/NPB3.4-OMP/bin

benchmark=$1
class=$2
pkg_power_limit=$3
dram_power_limit=$4
total_power_limit=$5

echo $0 $@

benchmark_path=$benchmark_dir/$benchmark.$class.x
if [ ! -e $benchmark_path ]; then
	echo "could not access $benchmark at $benchmark_path"
	exit -1
fi

# machine
machine=$(hostname -s)

result_dir=$basedir/results/$machine/$benchmark.$class.x
mkdir -p $result_dir

np=$(grep -c processor /proc/cpuinfo)
export OMP_NUM_THREADS=$np
export GOMP_CPU_AFFINITY="0-23"

# experiments
if [ "$#" -ne 5 ]; then
	echo "usage: $0 [app] [class] [total_power_limit] [pkg_power_limit] [dram_power_limit]"
	echo "   ex: $0 mg D 180 70 20"
	echo " note: total_power_limit = num_nodes * (pkg_power_limit + dram_power_limit)"
	exit -1
fi

echo "set cpu frequency to max frequency"
$basedir/utils/cpufreq-set.pl 2300000
echo "current cpu frequency"
$basedir/utils/cpufreq-get.pl

echo "setting power limit: pkg=$pkg_power_limit dram=$dram_power_limit"
sudo $powerlimitcmd -p $pkg_power_limit 
sudo $powerlimitcmd -r $dram_power_limit

sleep 10

#filenames
filename=P${pkg_power_limit}M${dram_power_limit}T${total_power_limit}
powerlog=$filename.pwr
perflog=$filename.perf

echo "starting power profiling"
echo "writing results to $result_dir/$powerlog and $result_dir/$perflog"
$raplcmd -f $result_dir/$powerlog -c 0,12 2>&1 1>/dev/null &
rapl_pid=$!
# 
echo start benchmark $benchmark_path
$benchmark_path 2>&1 1> $result_dir/$perflog
echo end benchmark

kill $rapl_pid

sudo $powerlimitcmd -p -1
sudo $powerlimitcmd -r -1

$basedir/utils/cpufreq-set.pl 1200000
sleep 10

