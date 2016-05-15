
CC = g++
MAKE = make

# define subsystems
CODEC = codec
EXAMPLES = examples
SYMBOL_DETECTOR = symbol_detector
LIB = lib

all:
	cd $(CODEC) && $(MAKE)
	cd $(SYMBOL_DETECTOR) && $(MAKE)
	cd $(EXAMPLES) && $(MAKE)
	
$(CODEC): $(CODEC)/codec.h $(CODEC)/codec.cpp
	cd $(CODEC) && $(MAKE)

$(SYMBOL_DETECTOR): $(SYMBOL_DETECTOR)/symbol_detector.h \
                  $(SYMBOL_DETECTOR)/symbol_detector.cpp
	cd $(SYMBOL_DETECTOR) && $(MAKE)
	
$(EXAMPLES): $(EXAMPLES)/decoder_example/decoder_example.cpp \
             $(EXAMPLES)/symbol_detector_example/symbol_detector_example.cpp
	cd $(EXAMPLES)/decoder_example && $(MAKE)
	cd $(EXAMPLES)/symbol_detector_example && $(MAKE)

$(LIB): $(LIB)/tochee/tochee.cpp
	cd $(LIB) && $(MAKE)

touch_all:
	find . -exec touch {} \;