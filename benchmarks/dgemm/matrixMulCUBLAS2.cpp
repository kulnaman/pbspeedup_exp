////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
//
// Please refer to the NVIDIA end user license agreement (EULA) associated
// with this source code for terms and conditions that govern your use of
// this software. Any use, reproduction, disclosure, or distribution of
// this software and related documentation outside the terms of the EULA
// is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////////

//
// Matrix multiplication: C = A * B.
// Host code.
//
// This sample implements matrix multiplication as described in Chapter 3
// of the programming guide and uses the CUBLAS library to demonstrate
// the best performance.
//
// CUBLAS provides high-performance matrix multiplication.
// See also:
// V. Volkov and J. Demmel, "Benchmarking GPUs to tune dense linear algebra,"
// in Proc. 2008 ACM/IEEE Conf. on Superconducting (SC '08),
// Piscataway, NJ: IEEE Press, 2008, pp. Art. 31:1-11.
//

// Utilities and system includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <helper_string.h>  // helper for shared functions common to CUDA SDK samples

#include <cblas.h>
//#include <mkl_cblas.h>
//#include <mkl_blas.h>
typedef int blasint;

// CUDA runtime
#include <cuda_runtime.h>
#include <cublas_v2.h>

// CUDA and CUBLAS functions
#include <helper_functions.h>

#ifndef min
#define min(a,b) ((a < b) ? a : b)
#endif
#ifndef max
#define max(a,b) ((a > b) ? a : b)
#endif
#define 	DEFAULT_NUM_REPEATS	100

typedef struct _matrixSize  // Optional Command-line multiplier for matrix sizes
{
	unsigned int uiWA, uiHA, uiHA2, uiWB, uiHB, uiWC, uiHC, uiHC2;
} sMatrixSize;

typedef struct ReportData {
	sMatrixSize size;
	double gflops_sys;
	double gflops_cpu;
	double gflops_gpu;
	double time_sys;
	double time_cpu;
	double time_gpu;
	int repeats;
} ReportData;

double compute_gflops(int widthA, int heightA, int widthB,
		double time_in_seconds) {
	double num_operations = 2.0 * widthA * heightA * widthB;
	double gflops = num_operations * 1.0e-9 / time_in_seconds;
	return gflops;
}

void report(ReportData& rep) {
	printf("%s=%d %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d\n", "A_width",
			rep.size.uiWA, "A_height_CPU", rep.size.uiHA2, "A_height_GPU",
			rep.size.uiHA, "B_width", rep.size.uiWB, "B_height", rep.size.uiHB,
			"CPU_Percent",
			int(100.0 * rep.size.uiHA2 / (rep.size.uiHA + rep.size.uiHA2)),
			"repeats", rep.repeats);

	rep.gflops_gpu = compute_gflops(rep.size.uiWA, rep.size.uiHA, rep.size.uiWB,
			rep.time_gpu);
	printf(
			"GPU Performance = %10.2f GFLOPS/s Time_Per_Iteration = %10.3f seconds Total_time = %10.3f seconds\n",
			rep.gflops_gpu, rep.time_gpu, rep.time_gpu * rep.repeats);

	rep.gflops_cpu = compute_gflops(rep.size.uiWA, rep.size.uiHA2,
			rep.size.uiWB, rep.time_cpu);
	printf(
			"CPU Performance = %10.2f GFLOPS/s Time_Per_Iteration = %10.3f seconds Total_time = %10.3f seconds\n",
			rep.gflops_cpu, rep.time_cpu, rep.time_cpu * rep.repeats);

	rep.gflops_sys = compute_gflops(rep.size.uiWA,
			(rep.size.uiHA + rep.size.uiHA2), rep.size.uiWB, rep.time_sys);
	printf(
			"SYS Performance = %10.2f GFLOPS/s  Time_Per_Iteration = %10.3f seconds Total_time = %10.3f seconds\n",
			rep.gflops_sys, rep.time_sys, rep.time_sys * rep.repeats);
}

void printMatrix(double *matrix, unsigned int w, unsigned int h) {
	int i = 0, j = 0;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			printf(" %.8d", *(matrix + i * w + j));
		}
		printf("\n");
	}
	printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
//! Compute reference data set matrix multiply on CPU
//! C = A * B
//! @param C          reference data, computed but preallocated
//! @param A          matrix A as provided to device
//! @param B          matrix B as provided to device
//! @param hA         height of matrix A
//! @param wB         width of matrix B
////////////////////////////////////////////////////////////////////////////////
void matrixMulCPU(double *C, const double *A, const double *B, const blasint hA,
		const blasint wA, const blasint wB) {
	for (unsigned int i = 0; i < hA; ++i)
		for (unsigned int j = 0; j < wB; ++j) {
			double sum = 0;

			for (unsigned int k = 0; k < wA; ++k) {
				double a = A[i * wA + k];
				double b = B[k * wB + j];
				sum += a * b;
			}

			C[i * wB + j] = (float) sum;
		}
}

//////////////////////////////////////////////////////////////////////////////////////
//! Compute reference data set matrix multiply on CPU using OpenBlas call cblas_sgemm
//! C = A * B
//! @param C          reference data, computed but preallocated
//! @param A          matrix A as provided to device
//! @param B          matrix B as provided to device
//! @param hA         height of matrix A
//! @param wA         width of matrix A
//! @param wB         width of matrix B
//////////////////////////////////////////////////////////////////////////////////////
void matrixMulBLAS(double *C, const double *A, const double *B,
		const blasint hA, const blasint wA, const blasint wB) {
	const double alpha = 1.0f;
	const double beta = 0.0f;

	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, hA, wB, wA, alpha, A,
			wA, B, wB, beta, C, wA);
}

////////////////////////////////////////////////////////////////////////////////
// These are CUDA Helper functions (in addition to helper_cuda.h)

void inline checkError(cublasStatus_t status, const char *msg) {
	if (status != CUBLAS_STATUS_SUCCESS) {
		printf("%s", msg);
		exit(EXIT_FAILURE);
	}
}
// end of CUDA Helper Functions

// Allocates a matrix with random float entries.
void randomInit(double *data, int size) {
	for (int i = 0; i < size; ++i)
		data[i] = rand() / (double) RAND_MAX;
	//data[i] = val;
}

void printDiff(double *data1, double *data2, int width, int height,
		int iListLength, double fListTol) {
	printf("Listing first %d Differences > %.6d...\n", iListLength, fListTol);
	int i, j, k;
	int error_count = 0;

	for (j = 0; j < height; j++) {
		if (error_count < iListLength) {
			printf("\n  Row %d:\n", j);
		}

		for (i = 0; i < width; i++) {
			k = j * width + i;
			double fDiff = fabs(data1[k] - data2[k]);

			if (fDiff > fListTol) {
				if (error_count < iListLength) {
					printf("    Loc(%d,%d)\tCPU=%.5d\tGPU=%.5d\tDiff=%.6d\n", i,
							j, data1[k], data2[k], fDiff);
				}

				error_count++;
			}
		}
	}

	printf(" \n  Total Errors = %d\n", error_count);
}

void initializeCUDA(int argc, char **argv, int &devID, int &iSizeMultiple,
		sMatrixSize &matrix_size, int &nrepeats) {
	// By default, we use device 0, otherwise we override the device ID based on what is provided at the command line

	if (checkCmdLineFlag(argc, (const char **) argv, "help")
			|| checkCmdLineFlag(argc, (const char **) argv, "?")) {
		printf("Usage -device=n (n >= 0 for deviceID)\n");
		printf("      -wA=WidthA -hA=HeightA (Width x Height of Matrix A)\n");
		printf("      -wB=WidthB -hB=HeightB (Width x Height of Matrix B)\n");
		printf(
				"Usage -CPU=%RanOnCPU -GPU=%RanOnGPU (only need one, if both GPU percent will proceed)");
		printf(
				"Usage -threads=? (choose how many threads you want GPU to run\n");
		printf(
				"Usage -repeats=n (choose how many repeats you want the benchmark to run\n");
		printf(
				"  Note: Outer matrix dimensions of A & B matrices must be equal.\n");

		exit(EXIT_SUCCESS);
	}
	cudaError_t error;
	devID = 0;

	if (checkCmdLineFlag(argc, (const char **) argv, "repeats")) {
		nrepeats = getCmdLineArgumentInt(argc, (const char **) argv, "repeats");
	} else {
		nrepeats = DEFAULT_NUM_REPEATS;
	}

	if (checkCmdLineFlag(argc, (const char **) argv, "device")) {
		devID = getCmdLineArgumentInt(argc, (const char **) argv, "device");
		error = cudaSetDevice(devID);

		if (error != cudaSuccess) {
			printf("cudaSetDevice returned error code %d, line(%d)\n", error,
					__LINE__);
			exit(EXIT_FAILURE);
		}
	}

	// get number of SMs on this GPU
	error = cudaGetDevice(&devID);

	if (error != cudaSuccess) {
		printf("cudaGetDevice returned error code %d, line(%d)\n", error,
				__LINE__);
		exit(EXIT_FAILURE);
	}

	if (checkCmdLineFlag(argc, (const char **) argv, "sizemult")) {
		iSizeMultiple = getCmdLineArgumentInt(argc, (const char **) argv,
				"sizemult");
	}

	iSizeMultiple = min(iSizeMultiple, 30);
	iSizeMultiple = max(iSizeMultiple, 1);

	cudaDeviceProp deviceProp;

	error = cudaGetDeviceProperties(&deviceProp, devID);

	if (error != cudaSuccess) {
		printf("cudaGetDeviceProperties returned error code %d, line(%d)\n",
				error, __LINE__);
		exit(EXIT_FAILURE);
	}

	printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID,
			deviceProp.name, deviceProp.major, deviceProp.minor);

	// use a larger block size for Fermi and above
	int block_size = (deviceProp.major < 2) ? 16 : 32;

	if (checkCmdLineFlag(argc, (const char **) argv, "threads")) {
		block_size = (int) getCmdLineArgumentInt(argc, (const char **) argv,
				"threads");
	}
	printf("block size = %d\n", block_size);
	matrix_size.uiWA = 2 * block_size * iSizeMultiple;
	matrix_size.uiHA = 2 * block_size * iSizeMultiple;
	matrix_size.uiHA2 = 2 * block_size * iSizeMultiple;
	matrix_size.uiWB = 2 * block_size * iSizeMultiple;
	matrix_size.uiHB = 4 * block_size * iSizeMultiple;
	matrix_size.uiWC = 2 * block_size * iSizeMultiple;
	matrix_size.uiHC = 2 * block_size * iSizeMultiple;
	matrix_size.uiHC2 = 2 * block_size * iSizeMultiple;

	double percentCPU = 0;
	double percentGPU = 1;

	if (checkCmdLineFlag(argc, (const char **) argv, "CPU")) {
		percentCPU = (double) getCmdLineArgumentInt(argc, (const char **) argv,
				"CPU");
		percentGPU = (100 - percentCPU) / 100;
		percentCPU = percentCPU / 100;
	}

	if (checkCmdLineFlag(argc, (const char **) argv, "GPU")) {
		percentGPU = (double) getCmdLineArgumentInt(argc, (const char **) argv,
				"GPU");
		percentCPU = (100 - percentGPU) / 100;
		percentGPU = percentGPU / 100;
	}

	// width of Matrix A
	if (checkCmdLineFlag(argc, (const char **) argv, "wA")) {
		matrix_size.uiWA = getCmdLineArgumentInt(argc, (const char **) argv,
				"wA");
	}

	// height of Matrix A
	if (checkCmdLineFlag(argc, (const char **) argv, "hA")) {
		matrix_size.uiHA = getCmdLineArgumentInt(argc, (const char **) argv,
				"hA");
		matrix_size.uiHA2 = getCmdLineArgumentInt(argc, (const char **) argv,
				"hA");
		matrix_size.uiHC = getCmdLineArgumentInt(argc, (const char **) argv,
				"hA");
		matrix_size.uiHC2 = getCmdLineArgumentInt(argc, (const char **) argv,
				"hA");
	}

	// width of Matrix B
	if (checkCmdLineFlag(argc, (const char **) argv, "wB")) {
		matrix_size.uiWB = getCmdLineArgumentInt(argc, (const char **) argv,
				"wB");
		matrix_size.uiWC = getCmdLineArgumentInt(argc, (const char **) argv,
				"wB");
	}

	// height of Matrix B
	if (checkCmdLineFlag(argc, (const char **) argv, "hB")) {
		matrix_size.uiHB = getCmdLineArgumentInt(argc, (const char **) argv,
				"hB");
	}

	if (matrix_size.uiWA != matrix_size.uiHB) {
		printf("Error: outer matrix dimensions must be equal. (%d != %d)\n",
				matrix_size.uiWA, matrix_size.uiHB);
		exit(EXIT_FAILURE);
	}

	matrix_size.uiHA = matrix_size.uiHA * percentGPU;
	matrix_size.uiHA2 = matrix_size.uiHA2 * percentCPU;
	matrix_size.uiHC = matrix_size.uiHC * percentGPU;
	matrix_size.uiHC2 = matrix_size.uiHC2 * percentCPU;

	printf("GPU: MatrixA(%u,%u), MatrixB(%u,%u), MatrixC(%u,%u)\n",
			matrix_size.uiWA, matrix_size.uiHA, matrix_size.uiWB,
			matrix_size.uiHB, matrix_size.uiWC, matrix_size.uiHC);
	printf("CPU: MatrixA2(%u,%u), MatrixB(%u,%u), MatrixC(%u,%u)\n",
			matrix_size.uiWA, matrix_size.uiHA2, matrix_size.uiWB,
			matrix_size.uiHB, matrix_size.uiWC, matrix_size.uiHC2);
}

////////////////////////////////////////////////////////////////////////////////
//! Run a simple test matrix multiply using CUBLAS
////////////////////////////////////////////////////////////////////////////////
int matrixMultiply(int argc, char **argv, int devID, sMatrixSize &matrix_size,
		int nrepeats, ReportData& rep) {
	cudaDeviceProp deviceProp;
	cudaError_t error;

	error = cudaGetDeviceProperties(&deviceProp, devID);

	if (error != cudaSuccess) {
		printf("cudaGetDeviceProperties returned error code %d, line(%d)\n",
				error, __LINE__);
		exit(EXIT_FAILURE);
	}

	// use a larger block size for Fermi and above
	int block_size = (deviceProp.major < 2) ? 16 : 32;
	if (checkCmdLineFlag(argc, (const char **) argv, "threads")) {
		block_size = (int) getCmdLineArgumentInt(argc, (const char **) argv,
				"threads");
	}
	printf("block size = %d\n", block_size);
	// set seed for rand()
	srand(2006);

	// allocate host memory for matrices A and B
	unsigned int size_A = matrix_size.uiWA * matrix_size.uiHA;
	unsigned int mem_size_A = sizeof(double) * size_A;
	double *h_A = (double *) malloc(mem_size_A);
	unsigned int size_A2 = matrix_size.uiWA * matrix_size.uiHA2;
	unsigned int mem_size_A2 = sizeof(double) * size_A2;
	double *h_A2 = (double *) malloc(mem_size_A2);
	unsigned int size_B = matrix_size.uiWB * matrix_size.uiHB;
	unsigned int mem_size_B = sizeof(double) * size_B;
	double *h_B = (double *) malloc(mem_size_B);

        if (h_A == NULL || h_A2 == NULL || h_B == NULL){
                printf("Host: Malloc failedi at %s:%d\n", __FILE__, __LINE__);
        }

	// set seed for rand()
	srand(2006);

	// initialize host memory
	randomInit(h_A, size_A);
	randomInit(h_A2, size_A2);
	randomInit(h_B, size_B);

	// allocate device memory
	double *d_A, *d_B, *d_C;
	unsigned int size_C = matrix_size.uiWC * matrix_size.uiHC;
	unsigned int mem_size_C = sizeof(double) * size_C;
	unsigned int size_C2 = matrix_size.uiWC * matrix_size.uiHC2;
	unsigned int mem_size_C2 = sizeof(double) * size_C2;
	unsigned int size_Cf = matrix_size.uiWC
			* (matrix_size.uiHC + matrix_size.uiHC2);
	unsigned int mem_size_Cf = sizeof(double) * size_Cf;

	// allocate host memory for the result
	double *h_C2 = (double *) malloc(mem_size_C2);
	double *h_CUBLAS = (double *) malloc(mem_size_C);
	double *h_Cf = (double *) malloc(mem_size_Cf);

	error = cudaMalloc((void **) &d_A, mem_size_A);

	if (error != cudaSuccess) {
		printf("cudaMalloc d_A returned error code %d, line(%d)\n", error,
				__LINE__);
		exit(EXIT_FAILURE);
	}

	error = cudaMalloc((void **) &d_B, mem_size_B);

	if (error != cudaSuccess) {
		printf("cudaMalloc d_B returned error code %d, line(%d)\n", error,
				__LINE__);
		exit(EXIT_FAILURE);
	}

	// copy host memory to device
	error = cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice);

	if (error != cudaSuccess) {
		printf("cudaMemcpy d_A h_A returned error code %d, line(%d)\n", error,
				__LINE__);
		exit(EXIT_FAILURE);
	}

	error = cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice);

	if (error != cudaSuccess) {
		printf("cudaMemcpy d_B h_B returned error code %d, line(%d)\n", error,
				__LINE__);
		exit(EXIT_FAILURE);
	}

	error = cudaMalloc((void **) &d_C, mem_size_C);

	if (error != cudaSuccess) {
		printf("cudaMalloc d_C returned error code %d, line(%d)\n", error,
				__LINE__);
		exit(EXIT_FAILURE);
	}

	// setup execution parameters
	dim3 threads(block_size, block_size);
	dim3 grid(matrix_size.uiWC / threads.x, matrix_size.uiHC / threads.y);

	// create and start timer
	printf("Computing result using CUBLAS...");

	// execute the kernel
	int nIter = nrepeats;

	// CUBLAS version 2.0
	{
		cublasHandle_t handle;

		cublasStatus_t ret;

		ret = cublasCreate(&handle);

		if (ret != CUBLAS_STATUS_SUCCESS) {
			printf("cublasCreate returned error code %d, line(%d)\n", ret,
					__LINE__);
			exit(EXIT_FAILURE);
		}

		const double alpha = 1.0f;
		const double beta = 0.0f;
		if (matrix_size.uiHA != 0) {
			//Perform warmup operation with cublas
			ret = cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N,
					matrix_size.uiWB, matrix_size.uiHA, matrix_size.uiWA,
					&alpha, d_B, matrix_size.uiWB, d_A, matrix_size.uiWA, &beta,
					d_C, matrix_size.uiWA);

			if (ret != CUBLAS_STATUS_SUCCESS) {
				printf("cublasSgemm returned error code %d, line(%d)\n", ret,
						__LINE__);
				exit(EXIT_FAILURE);
			}
		}
		// Allocate CUDA events that we'll use for timing
		cudaEvent_t start;
		error = cudaEventCreate(&start);

		if (error != cudaSuccess) {
			fprintf(stderr, "Failed to create start event (error code %s)!\n",
					cudaGetErrorString(error));
			exit(EXIT_FAILURE);
		}

		cudaEvent_t stop;
		error = cudaEventCreate(&stop);

		if (error != cudaSuccess) {
			fprintf(stderr, "Failed to create stop event (error code %s)!\n",
					cudaGetErrorString(error));
			exit(EXIT_FAILURE);
		}

		struct timeval begin, beginCPU, end, endCPU;
		gettimeofday(&begin, NULL);

		// Record the start event
		error = cudaEventRecord(start, NULL);

		if (error != cudaSuccess) {
			fprintf(stderr, "Failed to record start event (error code %s)!\n",
					cudaGetErrorString(error));
			exit(EXIT_FAILURE);
		}

		if (matrix_size.uiHA != 0) {
			for (int j = 0; j < nIter; j++) {
				//note cublas is column primary!
				//need to transpose the order
				ret = cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N,
						matrix_size.uiWB, matrix_size.uiHA, matrix_size.uiWA,
						&alpha, d_B, matrix_size.uiWB, d_A, matrix_size.uiWA,
						&beta, d_C, matrix_size.uiWA);

				if (ret != CUBLAS_STATUS_SUCCESS) {
					printf("cublasSgemm returned error code %d, line(%d)\n",
							ret, __LINE__);
					exit(EXIT_FAILURE);
				}
			}
		}

		printf("done.\n");

		// Record the stop event
		error = cudaEventRecord(stop, NULL);
		gettimeofday(&beginCPU, NULL);
		if (matrix_size.uiHA2 != 0) {
			for (int j = 0; j < nIter; j++) {
				matrixMulBLAS(h_C2, h_A2, h_B, matrix_size.uiHA2,
						matrix_size.uiWA, matrix_size.uiWB);
			}
		}
		gettimeofday(&endCPU, NULL);

		if (error != cudaSuccess) {
			fprintf(stderr, "Failed to record stop event (error code %s)!\n",
					cudaGetErrorString(error));
			exit(EXIT_FAILURE);
		}

		// Wait for the stop event to complete
		error = cudaEventSynchronize(stop);
		gettimeofday(&end, NULL);

		if (error != cudaSuccess) {
			fprintf(stderr,
					"Failed to synchronize on the stop event (error code %s)!\n",
					cudaGetErrorString(error));
			exit(EXIT_FAILURE);
		}

		float gpu_runtime = 0.0f;
		error = cudaEventElapsedTime(&gpu_runtime, start, stop);

		if (error != cudaSuccess) {
			fprintf(stderr,
					"Failed to get time elapsed between events (error code %s)!\n",
					cudaGetErrorString(error));
			exit(EXIT_FAILURE);
		}

		// Compute and print the performance
		float gpu_runtime_iter = gpu_runtime / nIter;
		double flopsPerMatrixMul = 2.0 * (double) matrix_size.uiWA
				* (double) matrix_size.uiHA * (double) matrix_size.uiWB;
		double gigaFlops = (flopsPerMatrixMul * 1.0e-9f)
				/ (gpu_runtime_iter / 1000.0);
		printf(
				"GPU Performance= %.2f GFlop/s, Time(perIter)= %.3f msec, Time(Total)= %.3f msec, Size= %.0f ps\n",
				gigaFlops, gpu_runtime_iter, gpu_runtime, flopsPerMatrixMul);

		// copy result from device to host
		error = cudaMemcpy(h_CUBLAS, d_C, mem_size_C, cudaMemcpyDeviceToHost);

		if (error != cudaSuccess) {
			printf("cudaMemcpy h_CUBLAS d_C returned error code %d, line(%d)\n",
					error, __LINE__);
			exit(EXIT_FAILURE);
		}

		checkError(cublasDestroy(handle), "cublasDestroy() error!\n");

		double cpu_runtime = (endCPU.tv_sec * 1000 + endCPU.tv_usec / 1000.0)
				- (beginCPU.tv_sec * 1000 + beginCPU.tv_usec / 1000.0);
		double flops_on_cpu = 2.0 * (double) matrix_size.uiWA
				* (double) matrix_size.uiHA2 * (double) matrix_size.uiWB;
		double cpu_gflops = (flops_on_cpu * 1.0e-9f)
				/ (cpu_runtime / 1000.0 / nIter);

		printf(
				"\nCPU Performance = %.2f GFlops, Time(perIter): %.3lf msec\t Time(Total) = %.3lf msec, # of operations = %.2g\n",
				cpu_gflops, cpu_runtime / nIter, cpu_runtime, cpu_gflops);
		double sys_runtime = (end.tv_sec * 1000 + end.tv_usec / 1000.0)
				- (begin.tv_sec * 1000 + begin.tv_usec / 1000.0);
		printf("\nSystem runtime(perIter): %.3lf msec\tTotal runtime: %.3lf\n",
				sys_runtime / nIter, sys_runtime);

		printf("\n%.2f	%.3f	%.3f	%.3lf	%.3lf	%.3lf	%.3lf\n", gigaFlops,
				gpu_runtime / nIter, gpu_runtime, cpu_runtime / nIter,
				cpu_runtime, sys_runtime / nIter, sys_runtime);

		// save the report data
		rep.size.uiHA = matrix_size.uiHA;
		rep.size.uiHA2 = matrix_size.uiHA2;
		rep.size.uiHB = matrix_size.uiHB;
		rep.size.uiWA = matrix_size.uiWA;
		rep.size.uiWB = matrix_size.uiWB;

		rep.repeats = nIter;
		rep.time_gpu = 1.0e-3 * gpu_runtime / nIter;
		rep.time_cpu = 1.0e-3 * cpu_runtime / nIter;
		rep.time_sys = 1.0e-3 * sys_runtime / nIter;
	}

	memcpy(h_Cf, h_CUBLAS, mem_size_C);
	memcpy(h_Cf + size_C, h_C2, mem_size_C2);

	// compute reference solution
	printf("Computing result using host CPU...\n");
	/*double *reference = (double *)malloc(mem_size_C);
	 matrixMulCPU(reference, h_A, h_B, matrix_size.uiHA, matrix_size.uiWA, matrix_size.uiWB);
	 printf("done.\n");

	 // check result (CUBLAS)
	 bool resCUBLAS = sdkCompareL2fe(reference, h_CUBLAS, size_C, 1.0e-2f);

	 if (resCUBLAS != true)


	 {
	 printDiff(reference, h_CUBLAS, matrix_size.uiWC, matrix_size.uiHC, 100, 1.0e-2f);
	 }

	 printf("Comparing CUBLAS Matrix Multiply with CPU results: %s\n", (true == resCUBLAS) ? "OK" : "FAIL");
	 double *reference2 = (double *)malloc(mem_size_C2);
	 matrixMulCPU(reference, h_A2, h_B, matrix_size.uiHA2, matrix_size.uiWA, matrix_size.uiWB);
	 printf("done.\n");

	 // check result (CUBLAS)
	 resCUBLAS = sdkCompareL2fe(reference, h_C2, size_C2, 1.0e-2f);

	 if (resCUBLAS != true)
	 {
	 printDiff(reference, h_C2, matrix_size.uiWC, matrix_size.uiHC2, 100, 1.0e-2f);
	 }

	 printf("Comparing BLAS Matrix Multiply with CPU results: %s\n", (true == resCUBLAS) ? "OK" : "FAIL");
	 */

	// clean up memory
	free(h_A);
	free(h_B);
	free(h_CUBLAS);
	free(h_C2);
	free(h_Cf);
	// free(reference);
	// free(reference2);
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);

	cudaDeviceReset();

	//  if (resCUBLAS == true)
	// {
	return EXIT_SUCCESS;    // return value = 1
	// }
	// else
	// {
	//     return EXIT_FAILURE;     // return value = 0
	// }
}

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	printf("[Matrix Multiply CUBLAS] - Starting...\n");

	int devID = 0, sizeMult = 5;
	sMatrixSize matrix_size;

	int nrepeats = DEFAULT_NUM_REPEATS;

	initializeCUDA(argc, argv, devID, sizeMult, matrix_size, nrepeats);

	ReportData rep;
	int matrix_result = matrixMultiply(argc, argv, devID, matrix_size, nrepeats,
			rep);
	report(rep);

	exit(matrix_result);
}
