import numpy as np
import cv2
import tochee

ti = tochee.Tochee(5) # create a comms interface a 1 bit-per-symbol

cap = cv2.VideoCapture(1)

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
		ti.decoder_write(symbol_list[0].value)
	
	data = ti.decoder_read(1)
	
	for byte in data:
		print("0x%02X" % byte)
		
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break