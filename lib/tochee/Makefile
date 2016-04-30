
BASE_DIR = ../..
OBJ = $(BASE_DIR)/symbol_detector/symbol_detector.o $(BASE_DIR)/codec/codec.o
LIB = -lopencv_core -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_calib3d

all: tochee.cpp
	g++ -o libtochee.so -I$(BASE_DIR) -shared -fPIC tochee.cpp $(OBJ) $(LIB)
