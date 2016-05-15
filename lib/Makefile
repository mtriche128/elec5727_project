
BASE_DIR = ../
OBJ_ALL = $(BASE_DIR)/codec/codec.o
OBJ_CPU = $(BASE_DIR)/symbol_detector/symbol_detector.o
OBJ_GPU = $(BASE_DIR)/symbol_detector/symbol_detector_GPU.o
LIB = -lopencv_core -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_calib3d -lopencv_contrib
LIB_GPU = -lopencv_gpu

all: libtochee_cpu.cpp libtochee_gpu.cpp
	g++ -o libtochee_cpu.so -I$(BASE_DIR) -shared -fPIC libtochee_cpu.cpp $(OBJ_ALL) $(OBJ_CPU) $(LIB)
	g++ -o libtochee_gpu.so -I$(BASE_DIR) -shared -fPIC libtochee_gpu.cpp $(OBJ_ALL) $(OBJ_GPU) $(LIB) $(LIB_GPU)
