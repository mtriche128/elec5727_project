CC = g++

all: cpu gpu

gpu: symbol_detector_GPU.h symbol_detector_GPU.cpp features.h
	$(CC) -std=c++11 -c symbol_detector_GPU.cpp -fPIC

cpu: symbol_detector.h symbol_detector.cpp features.h
	$(CC) -std=c++11 -c symbol_detector.cpp -fPIC

clean: 
	rm *.o
