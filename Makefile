
CC = g++
MAKE = make

# define subsystems
CODEC = codec
SYMBOL_DETECTOR = symbol_detector
LIB = lib
EXPERIMENTS = experiments
SCRIPTS = scripts

all:
	cd $(CODEC) && $(MAKE)
	cd $(SYMBOL_DETECTOR) && $(MAKE)
	cd $(LIB) && $(MAKE)

install:
	cd $(EXPERIMENTS) && $(MAKE)
	cd $(SCRIPTS) && $(MAKE)

touch_all:
	find . -exec touch {} \;