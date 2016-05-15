# -----------------------------------------------------------------------------
# Matthew Triche
# ELEC5727
# Final Project
#
# This script tests the encoder by transmitting a symbol sequence using the screen.
# -----------------------------------------------------------------------------

import numpy as np
import cv2
import tochee_cpu
import array
import time
import os

BPS = 2 # bits-per-symbol
FPS = 5 # TX frames-per-second
EOF_SYM = 40 # end-of-frame symbol
MEDIA_PATH = "../media"

ABS_MEDIA_PATH = os.path.abspath(MEDIA_PATH)

ti = tochee_cpu.Tochee(BPS)

symbol_queue = []

def write_frame(tochee_int, data, symq):
	tochee_int.encoder_write(data)
	
	# consume the entire encoder output
	while(1):
		s = ti.encoder_read(4)
		if len(s) == 0:
			break
		symq += s
	
	symq.append(EOF_SYM)
	print(str(symq))


write_frame(ti, [0xAB, 0xCD, 0x1F], symbol_queue)
#write_frame(ti, [0x81, 0x00, 0x55, 0xAA], symbol_queue)
#write_frame(ti, [0xAB, 0xCD, 0x1F], symbol_queue)
#write_frame(ti, [0x81, 0x00, 0x55, 0xAA], symbol_queue)
#write_frame(ti, [0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00], symbol_queue)
#write_frame(ti, [ord('I'),ord('T'),ord(' '),ord('W'),ord('O'),ord('R'),ord('K'),ord('S'),ord('!')], symbol_queue)

t0 = cv2.getTickCount()

sym_index = 0
while(1):	
	t_delta = (cv2.getTickCount()-t0)/cv2.getTickFrequency()
	if t_delta >= (1.0/float(FPS)):
		t0 = cv2.getTickCount()
		if sym_index == len(symbol_queue):
			#break
			sym_index = 0
		sym = symbol_queue[sym_index]
		img_path = os.path.join(ABS_MEDIA_PATH, str(sym)+".png")
		img = cv2.imread(img_path)
		if img == None:
			print("ERROR: Failed to read symbol image!")
			break
		print(img_path)
		bordered_img = cv2.copyMakeBorder(img,200,200,200,200,cv2.BORDER_CONSTANT,value=[255,255,255])
		cv2.imshow('transmitter', bordered_img)
		sym_index += 1

	if cv2.waitKey(1) & 0xFF == ord('q'):
		break
