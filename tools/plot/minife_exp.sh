#!/bin/bash -xe

./experiment.py ../../benchmarks/miniFE-omp/src/miniFE-400-200-100-cpu/*
mv miniFE.csv miniFE-cpu.csv
./experiment.py ../../benchmarks/miniFE-cuda/src/miniFE-400-200-100-gpu/*
mv miniFE.csv miniFE-gpu.csv

./fig1.py miniFE-gpu.csv
./fig2.py miniFE-cpu.csv miniFE-gpu.csv
