# -----------------------------------------------------------------------------
# Matthew Triche
# ELEC5727
# Final Project
#
# Experiment - Individual Symbol Tracking
# -----------------------------------------------------------------------------

import numpy as np
import cv2
import cv2.cv as cv
import tochee_cpu
import os
import json
import matplotlib.pyplot as plt

ti = tochee_cpu.Tochee(2)
dataset_path = os.path.join(os.getcwd(), "dataset") 
dir_list = os.listdir(dataset_path) # get list of image directories

# -----------------------------------------------------------------------------
# process data

result_list = []

print("%20s %10s %10s %10s %10s %12s" % ("image", "distance", "height", "width", "sym_count", "time"))

for img_dir in dir_list:
	img_dir_path = os.path.join(dataset_path, img_dir)
	
	if not(os.path.isdir(img_dir_path)):
		continue
	
	#print("--------------------------------")
	#print(img_dir)
	metaf = open(os.path.join(img_dir_path,"meta"),'r')
	metad = json.loads(metaf.read())
	#print("distance = %s" % metad["distance"])
	
	for img_file in os.listdir(img_dir_path):
		img_file_path = os.path.join(img_dir_path, img_file)
		
		if not(os.path.isfile(img_file_path)):
			continue
		
		base, ext = os.path.splitext(img_file_path)
		
		if not( (ext == ".jpg") or \
			(ext == ".png") or \
			(ext == ".JPG") ):
			continue
		
		img = cv2.imread(img_file_path)
		rows, cols, ch = img.shape
		
		#print("  * %s" % img_file)
		#print("     - width : %i" % cols) 
		#print("     - height: %i" % rows)
		
		t0 = cv2.getTickCount()
		sym_list = ti.symdet_push(img)
		t_delta = (cv2.getTickCount() - t0) / cv2.getTickFrequency()
		
		print("%20s %10i %10i %10i %10i %12.3f" % (img_file, metad['distance'], rows, cols, len(sym_list), t_delta))
		
		result = dict()
		result['distance'] = metad['distance']
		result['width']    = cols
		result['height']   = rows
		result['time']     = t_delta
		result['count']    = len(sym_list)
		result_list.append(result)

# -----------------------------------------------------------------------------
# display data : distance vs. execution time vs. image size

dist_data = dict()

for result in result_list:
	mpix = round(result['height']*result['width']/1e6,1)
	dist = result['distance']
	time = 1000*result['time']
	
	if mpix in dist_data.keys():
		dist_data[mpix].append( (dist, time) )
	else:
		dist_data[mpix] = [(dist, time)]

fig = plt.figure()
g1 = fig.add_subplot("211")

#fig, tgraph = plt.subplots()

for mpix in dist_data:
	dist_data[mpix] = sorted(dist_data[mpix])
	x_data, y_data = zip(*dist_data[mpix])
	g1.plot(np.array(x_data),np.array(y_data), label=str(mpix)+" MP")

g1.set_xlabel("Distance (in)")
g1.set_ylabel("Execution Time (ms)")
g1.set_title("Distance vs. Execution Time vs. Image Size")
legend = g1.legend(loc='upper left', shadow=True)

# -----------------------------------------------------------------------------
# display data: symbols found vs. distance

x_data = []
y_data = []

for result in result_list:
	dist  = result['distance']
	count = result['count']
	x_data.append(dist)
	y_data.append(count)

g2 = fig.add_subplot("212")
g2.scatter(np.array(x_data),np.array(y_data), marker='x')
g2.set_xlabel("Distance (in)")
g2.set_ylabel("Symbols Found")
g2.set_title("Symbols Found vs. Distance")

plt.tight_layout()
plt.show()

