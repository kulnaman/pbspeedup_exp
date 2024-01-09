#!/bin/bash
echo "running experiment"
echo $1
./bin/benchmark conv_example.txt out.csv fp32 0 1 $1 NCHW NCHW NCHW &> log.log

echo "Done"
python parse_result/parser.py out.csv -iter $1
