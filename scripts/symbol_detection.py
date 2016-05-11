# -----------------------------------------------------------------------------
# file : symbol_detection.py
# brief: This script allows symbol detection to be tested on single image frames.
# -----------------------------------------------------------------------------

import argparse
import tochee
import numpy as np
import cv2
import time

# -----------------------------------------------------------------------------
# Parse Arguments

parser = argparse.ArgumentParser(description='Symbol detection test.')

parser.add_argument("input_image",  help="path to the input image")
parser.add_argument("output_image", help="path to the input image")
parser.add_argument("-p", dest="process_type", help="which process to use: 'OpenCL' or 'CPU'", default="OpenCL")

args = parser.parse_args()
input_image  = args.input_image
output_image = args.output_image
process_type = args.process_type

print("input image path : %s" % input_image)
print("output image path: %s" % output_image)
print("process type     : %s" % process_type) 

# -----------------------------------------------------------------------------
# Initialize and execute image processing.

ti = tochee.Tochee(5) # initialize tochee interface

mst = cv2.imread(input_image)
img = np.copy(mst)

if img == None:
	print("ERROR: Unable to read input image.")
	exit()

symbol_list = ti.symdet_push(img)

for sym in symbol_list:
	# draw a line around each symbol
	c0 = (sym.corner_col[0], sym.corner_row[0])
	c1 = (sym.corner_col[1], sym.corner_row[1])
	c2 = (sym.corner_col[2], sym.corner_row[2])
	c3 = (sym.corner_col[3], sym.corner_row[3])
	cv2.line(img,c0,c1,(0,255,0),2)
	cv2.line(img,c1,c2,(0,255,0),2)
	cv2.line(img,c2,c3,(0,255,0),2)
	cv2.line(img,c3,c0,(0,255,0),2)
		
	center = (sym.center_col-5, sym.center_row+5)
	cv2.putText(img,str(sym.value),center, cv2.FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),3)
	cv2.putText(img,str(sym.value),center, cv2.FONT_HERSHEY_SIMPLEX, 0.5,(0,0,255),2)
	
print("symbols found    : %i" % len(symbol_list)) 
cv2.imwrite(output_image, img)
