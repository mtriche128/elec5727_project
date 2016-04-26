
CC=g++
INCLUDE=../..
SYMBOL_DETECTOR=../../symbol_detector

all: symbol_detector_example.cpp
	$(CC) -std=c++11 -o symbol_detector_example -I$(INCLUDE) symbol_detector_example.cpp $(SYMBOL_DETECTOR)/symbol_detector.o -lopencv_core -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_calib3d

