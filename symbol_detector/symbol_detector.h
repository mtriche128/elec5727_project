 /**************************************************************************//**
 * Matthew Triche
 * ELEC5727
 * Final Project
 * 
 * @file  symbol_detector.h
 * @brief Header file for the symbol detection kernel.
 * 
 * The symbol detector is designed to read in image frames and output whatever
 * target symbols may be present within.
 * 
 * CITATION:
 * This code is derived from Skyler Saleh's "Fiducial Based Visual Servoing"
 * software. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 ******************************************************************************/
 
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
using namespace std;
using namespace cv;
 
#ifndef _SYMBOL_DETECTOR_H
#define _SYMBOL_DETECTOR_H
 
/* ------------------------------------------------------------------------- *
 * Declare External Data                                                     *
 * ------------------------------------------------------------------------- */
 
struct Marker{
	Mat normalized;
	Mat res;
	Point2f center_norm;
	Point2f top_norm;
	Point2f center;
	Point2f top;
	float add_z;
	Mat affine;
	vector<Point>contour;
	vector<Point2f>float_contour;
	
	int petal_vals[8];
	int number;
	void calc_number(void);
};

/* ------------------------------------------------------------------------- *
 * Declare External Classes                                                     *
 * ------------------------------------------------------------------------- */

class SymbolDetector
{
public:
	SymbolDetector(void) { }
	~SymbolDetector(void);
	
	void push(const Mat &input, vector<Marker> &output);
};

#endif