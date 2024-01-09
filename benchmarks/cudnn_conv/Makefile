CXX := /usr/local/cuda-11.2/bin/nvcc
TARGET := conv
CUDNN_PATH := /usr/local/cuda-11.2
HEADERS := -I $(CUDNN_PATH)/include -I/usr/local/include/opencv4

LIBS := -L $(CUDNN_PATH)/lib64 -L/usr/local/lib 
CXXFLAGS := -arch=sm_60 -std=c++11 -O2

all: conv

conv: $(TARGET).cu
	$(CXX) $(CXXFLAGS) $(HEADERS) $(LIBS) $(TARGET).cu -o $(TARGET) \
	-lcudnn -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

.phony: clean

clean:
	rm $(TARGET) || echo -n ""
