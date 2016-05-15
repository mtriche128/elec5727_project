import numpy as np
import cv2
import tochee
import cv2.cv as cv

ti = tochee.Tochee(2) # create a comms interface a 1 bits-per-symbol

cap = cv2.VideoCapture(1)
#cap.set(cv.CV_CAP_PROP_FRAME_WIDTH, 800)
#cap.set(cv.CV_CAP_PROP_FRAME_HEIGHT,600)

EOF_SYM = 40

def output_byte(byte):
	print("0x%02X %11s %s" % (byte, '0b{0:08b}'.format(byte), chr(byte)))

while(1):
	ret, frame = cap.read()
	
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
			print('--------------------------')
			print('EOF:')
			while(1):
				data = ti.decoder_read(1)
				if len(data) != 0:
					output_byte(data[0])
				else:
					break
			print('pre-size = %i' % ti.decoder_size())
			# clear the decoder
			ti.decoder_clear()
			print('post-size = %i' % ti.decoder_size())
			print('--------------------------')
			
	data = ti.decoder_read(1)
	for byte in data:
		output_byte(byte)
		
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break