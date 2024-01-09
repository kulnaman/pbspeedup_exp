#!/bin/bash -xe

./experiment.py ../../benchmarks/dgemm/dgemm-8192-cpu/*
mv dgemm.csv dgemm-cpu.csv
./experiment.py ../../benchmarks/dgemm/dgemm-8192-gpu/*
mv dgemm.csv dgemm-gpu.csv


./fig1.py dgemm-gpu.csv
./fig1-cpu.py dgemm-cpu.csv
./fig2.py dgemm-cpu.csv dgemm-gpu.csv

