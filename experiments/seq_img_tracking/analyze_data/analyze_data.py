# -----------------------------------------------------------------------------
# Matthew Triche
# ELEC5727
# Final Project
#
# Experiment - Sequential Symbol Tracking
# -----------------------------------------------------------------------------

import numpy as np
import cv2
import tochee_cpu
import cv2.cv as cv
import matplotlib.pyplot as plt
import os
import json

EOF_SYM      = 40
DATASET_PATH = "dataset"
INDEX_PATH   = os.path.join(DATASET_PATH, "index.json") # load FPS and video information

# build test sequence
TEST_SEQ = []
for i in range(10):
	for b in [0xFF, 0x00, 0xAA, 0x55]:
		TEST_SEQ.append(b)

ti = tochee_cpu.Tochee(2) # create a comms interface a 1 bits-per-symbol

def output_byte(byte):
	print("0x%02X %11s %s" % (byte, '0b{0:08b}'.format(byte), chr(byte)))
	
def error(byte,test):
	diff = byte ^ test
	err = sum(int(ch) for ch in '{0:08b}'.format(diff))
	print("err: %02X => %i" % (test,err))
	return err

index_file = open(INDEX_PATH, 'r')
index = json.load(index_file)

x_data = []
y_data = []

videos = sorted(zip(index.values(), index.keys()))

for vid in videos:
	print("processing " + vid[1])
	
	video_path = os.path.join(DATASET_PATH, vid[1])
	cap = cv2.VideoCapture(video_path)
	rx_data = []
	err_sum = 0
	byte_count = 0
	error_count = 0
	test_index = 0
	
	while(1):
		ret, frame = cap.read()
		
		if frame == None:
			break # all frames have been consumed
		
		symbol_list = ti.symdet_push(frame)
		
		for sym in symbol_list:
			# draw a line around each symbol
			c0 = (sym.corner_col[0], sym.corner_row[0])
			c1 = (sym.corner_col[1], sym.corner_row[1])
			c2 = (sym.corner_col[2], sym.corner_row[2])
			c3 = (sym.corner_col[3], sym.corner_row[3])
			cv2.line(frame,c0,c1,(0,255,0),3)
			cv2.line(frame,c1,c2,(0,255,0),3)
			cv2.line(frame,c2,c3,(0,255,0),3)
			cv2.line(frame,c3,c0,(0,255,0),3)
			
			center = (sym.center_col-5, sym.center_row+5)
			cv2.putText(frame,str(sym.value),center, cv2.FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),3)
			cv2.putText(frame,str(sym.value),center, cv2.FONT_HERSHEY_SIMPLEX, 0.5,(0,0,255),2)
		
		cv2.imshow('output',frame)
		
		if len(symbol_list) == 1:
			
			if not( symbol_list[0].value == 0 or \
				symbol_list[0].value == 1 or \
				symbol_list[0].value == 2 or \
				symbol_list[0].value == 3 or \
				symbol_list[0].value == 4 or \
				symbol_list[0].value == 40 ):
				continue
			
			ti.decoder_write(symbol_list[0].value)
			if symbol_list[0].value == EOF_SYM:
				# consume the rest of the decoder output possible
				while(1):
					data = ti.decoder_read(1)
					if len(data) != 0:
						output_byte(data[0])
						rx_data.append(data[0])
						error_count += error(data[0], TEST_SEQ[test_index])
						byte_count += 1
						test_index += 1
					else:
						break
				# clear the decoder
				ti.decoder_clear()
				test_index = 0
				
		data = ti.decoder_read(1)
		for byte in data:
			output_byte(byte)
			rx_data.append(byte)
			error_count += error(byte, TEST_SEQ[test_index])
			byte_count += 1
			test_index += 1

		if cv2.waitKey(1) & 0xFF == ord('q'):
			break

	# add errors due to missing or extra bits
	error_count += abs(len(rx_data) - len(TEST_SEQ))

	total_bits = float(8*len(TEST_SEQ))
	
	print("error count = %i" % error_count)

	y_data.append(100*float(error_count)/total_bits)
	x_data.append(vid[0])
	
plt.plot(np.array(x_data), np.array(y_data))
plt.xlabel("Transmitter FPS")
plt.ylabel("Bit Error Rate (%)")
plt.title("Transmitter FPS vs. Bit Error Rate")
plt.show()


"""
		ret, frame = cap.read()
		
		if frame == None:
			BER = (float(err_sum) / (8.0*float(byte_count)))
			print("BER = %f" % BER)
			
			x_data.append(vid[1])
			y_data.append(BER)
			
			break
	
		symbol_list = ti.symdet_push(frame)
		
		for sym in symbol_list:
			# draw a line around each symbol
			c0 = (sym.corner_col[0], sym.corner_row[0])
			c1 = (sym.corner_col[1], sym.corner_row[1])
			c2 = (sym.corner_col[2], sym.corner_row[2])
			c3 = (sym.corner_col[3], sym.corner_row[3])
			cv2.line(frame,c0,c1,(0,255,0),3)
			cv2.line(frame,c1,c2,(0,255,0),3)
			cv2.line(frame,c2,c3,(0,255,0),3)
			cv2.line(frame,c3,c0,(0,255,0),3)
			
			center = (sym.center_col-5, sym.center_row+5)
			cv2.putText(frame,str(sym.value),center, cv2.FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),3)
			cv2.putText(frame,str(sym.value),center, cv2.FONT_HERSHEY_SIMPLEX, 0.5,(0,0,255),2)
		
		cv2.imshow('output',frame)
		data = ti.decoder_read(4)
		
		if len(symbol_list) == 1:
			ti.decoder_write(symbol_list[0].value)
		
		for byte in data:
			err = 8 - sum(int(ch) for ch in '{0:08b}'.format(byte))
			err_sum += err
			byte_count += 1
			print("0x%02X %11s %s %i" % (byte, '0b{0:08b}'.format(byte), chr(byte), err))
			
		if cv2.waitKey(1) & 0xFF == ord('q'):
			break
	
	cap.release()

plt.plot(np.array(x_data), np.array(y_data))
plt.xlabel("Transmitter FPS")
plt.ylabel("Bit Error Rate")
plt.title("Transmitter FPS vs. Bit Error Rate")
plt.show()
"""