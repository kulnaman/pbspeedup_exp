#!/bin/bash -xe

./fig5.py HPCG-gpu.csv
mv ./fig5.svg ./fig5/HPCG_fig5.svg
#./fig6.py HPCG-gpu.csv
#mv ./fig6.svg ./fig6/HPCG_fig6.svg
#./fig7.py HPCG-gpu.csv
#mv ./fig7.svg ./fig7/HPCG_fig7.svg
#./fig8.py HPCG-gpu.csv
#mv ./fig8.svg ./fig8/HPCG_fig8.svg

#./fig4.py cufft-gpu.csv
#mv ./fig4.svg ./fig4/cufft_fig4.svg
./fig5.py cufft-gpu.csv
mv ./fig5.svg ./fig5/cufft_fig5.svg
#./fig6.py cufftgpu.csv
#mv ./fig6.svg ./fig6/cufft_fig6.svg
#./fig7.py cufft-gpu.csv
#mv ./fig7.svg ./fig7/cufft_fig7.svg
#./fig8.py cufft-gpu.csv
#mv ./fig8.svg ./fig8/cufft_fig8.svg

#./fig4.py stream-gpu.csv
#mv ./fig4.svg ./fig4/stream_fig4.svg
./fig5.py stream-gpu.csv
mv ./fig5.svg ./fig5/streamfig5.svg
#./fig6.py stream-gpu.csv
#mv ./fig6.svg ./fig6/stream_fig6.svg
#./fig7.py stream-gpu.csv
#mv ./fig7.svg ./fig7/stream_fig7.svg
#./fig8.py stream-gpu.csv
#mv ./fig8.svg ./fig8/stream_fig8.svg

#./fig4.py cloverleaf-gpu.csv
#mv ./fig4.svg ./fig4/clover_leaf_fig4.svg
./fig5.py cloverleaf-gpu.csv
mv ./fig5.svg ./fig5/cloverleaf_fig5.svg
#./fig6.py cloverleaf-gpu.csv
#mv ./fig6.svg ./fig6/cloverleaf_fig6.svg
#./fig7.py cloverleaf-gpu.csv
#mv ./fig7.svg ./fig7/cloverleaf_fig7.svg
#./fig8.py cloverleaf-gpu.csv
#mv ./fig8.svg ./fig8/cloverleaf_fig8.svg

#./fig4.py cublas-gpu.csv
#mv ./fig4.svg ./fig4/cublas_fig4.svg
./fig5.py cublas-gpu.csv
mv ./fig5.svg ./fig5/cublas_fig5.svg
#./fig6.py cublas-gpu.csv
#mv ./fig6.svg ./fig6/cublas_fig6.svg
#./fig7.py cublas-gpu.csv
#mv ./fig7.svg ./fig7/cublas_fig7.svg
#./fig8.py cublas-gpu.csv
#mv ./fig8.svg ./fig8/cublas_fig8.svg