@author Ryan Vogt

Edited the original matrixMulCUBLAS to run on CPU and GPU

CPU runs with OpenBLAS
Added
-printMatrix function 
-matrixMulBLAS function
-In initializeCUDA
	-specify size of matrices, percent on CPU/GPU, device, and blocksize all on command line
	-added CPU timing functions
-matrixMulCUBLAS runs with sgemm
-matrixMulCUBLAS2 runs with dgemm 
#-changed nIter to 100

to run:
	-/matrixMulCUBLAS -wA=3200 -hA=3200 -wB=3200 -hB=3200 -CPU=50 -device=0 -threads=96 -repeats=100


Edited MakeFile to find openBLAS directories and cuda-5.0 directories
