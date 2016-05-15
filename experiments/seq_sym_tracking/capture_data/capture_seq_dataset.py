import numpy as np
import cv2
import tochee_cpu
import cv2.cv as cv
import os
import json

# -----------------------------------------------------------------------------
# define parameters

DATASET_PATH = "dataset"
BPS          = 2
TX_FPS_LIST  = [1,2,3,4,5,6,7,8,9,10,15,20,25,30]
EOF_SYM      = 40
MEDIA_PATH   = "../../../media"
FRAME_COUNT  = 10

# -----------------------------------------------------------------------------
# define functions

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
	
# -----------------------------------------------------------------------------
# define data

ti = tochee_cpu.Tochee(BPS)
cap = cv2.VideoCapture(1)
cap.set(cv.CV_CAP_PROP_FRAME_WIDTH, 800)
cap.set(cv.CV_CAP_PROP_FRAME_HEIGHT,600)

symbol_queue = []
index = dict() # file index

# -----------------------------------------------------------------------------
# run process

ABS_MEDIA_PATH = os.path.abspath(MEDIA_PATH)
# define test sequence
write_frame(ti, [0xFF, 0x00, 0xAA, 0x55], symbol_queue)

for fps in TX_FPS_LIST:
	fname = str(fps)+"fps.avi"
	index[fname] = fps
	vidout = cv2.VideoWriter(fname, cv.CV_FOURCC('F','M','P','4'), 30, (800,600), True)
	for i in range(FRAME_COUNT):
		sym_index = 0
		t0 = cv2.getTickCount()
		while(1):
			t_delta = (cv2.getTickCount()-t0)/cv2.getTickFrequency()
			if t_delta >= (1.0/float(fps)):
				t0 = cv2.getTickCount()
				if sym_index == len(symbol_queue):
					break
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
				exit()

			ret, frame = cap.read()
			vidout.write(frame)
	vidout.release()

fout = open('index.json','w')
fout.write(json.dumps(index))
fout.close()

