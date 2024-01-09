#-----------------------------------------------------------------------

MINIFE_TYPES =  \
        -DMINIFE_SCALAR=double   \
        -DMINIFE_LOCAL_ORDINAL=int      \
        -DMINIFE_GLOBAL_ORDINAL=int

MINIFE_MATRIX_TYPE = -DMINIFE_ELL_MATRIX

#-----------------------------------------------------------------------

CFLAGS = -O3 -I/usr/lib/x86_64-linux-gnu/openmpi/include/
CXXFLAGS = -O3 -I/usr/lib/x86_64-linux-gnu/openmpi/include/

# For debugging, the macro MINIFE_DEBUG will cause miniFE to dump a log file
# from each proc containing various information.
# This macro will also enable a somewhat expensive range-check on indices in
# the exchange_externals function.

#CPPFLAGS = -I. -I../utils -I../fem $(MINIFE_TYPES) $(MINIFE_MATRIX_TYPE) -DHAVE_MPI -DMPICH_IGNORE_CXX_SEEK  #-DMINIFE_DEBUG
CPPFLAGS = -I. -I../utils -I../fem $(MINIFE_TYPES) $(MINIFE_MATRIX_TYPE) -DHAVE_MPI -DMPICH_IGNORE_CXX_SEEK -DMATVEC_OVERLAP #-DGPUDIRECT #-DMINIFE_DEBUG
#CPPFLAGS = -I. -I../utils -I../fem $(MINIFE_TYPES) $(MINIFE_MATRIX_TYPE) #-DHAVE_MPI -DMPICH_IGNORE_CXX_SEEK -DMATVEC_OVERLAP #-DGPUDIRECT #-DMATVEC_OVERLAP #-DMINIFE_DEBUG
#CPPFLAGS = -I. -I../utils -I../fem $(MINIFE_TYPES) $(MINIFE_MATRIX_TYPE) -DHAVE_MPI -DMPICH_IGNORE_CXX_SEEK -DGPUDIRECT -DMATVEC_OVERLAP #-DMINIFE_DEBUG

LDFLAGS= 
LIBS= -l nvToolsExt

# The MPICH_IGNORE_CXX_SEEK macro is required for some mpich versions,
# such as the one on my cygwin machine.

#mvapich
#MPICFLAGS=-I/usr/local/mvapich/include -pthread 
#MPILDFLAGS=-L/usr/local/mvapich/lib -lmpichcxx -lmpich -lopa -lmpl -lcudart -lcuda -libverbs -ldl -lrt -lm -lpthread
#c++ -L/usr/local/cuda//lib64 -L/usr/local/cuda//lib -L/lib -L/lib -L/lib -Wl,-rpath,/lib -L/lib -Wl,-rpath,/lib -L/lib -Wl,-rpath,/lib -L/lib -L/lib -I/usr/local/mvapich/include -L/usr/local/mvapich/lib -lmpichcxx -lmpich -lopa -lmpl -lcudart -lcuda -libverbs -ldl -lrt -lm -lpthread

#openmpi
MPICFLAGS=-I$(MPI_HOME)/include -pthread 
MPILDFLAGS=-L$(MPI_HOME)/lib -lmpi -lmpi_cxx -ldl -lm -lrt -lnsl -lutil -lm -ldl

NVCCFLAGS=-lineinfo -gencode=arch=compute_75,code=\"sm_75,compute_75\" -gencode=arch=compute_70,code=\"sm_70,compute_70\" -gencode=arch=compute_60,code=\"sm_60,compute_60\" -gencode=arch=compute_61,code=\"sm_61,compute_61\"

NVCC=nvcc -Xcompiler -fopenmp 
CXX=mpicxx
CC=mpiccc

include make_targets
