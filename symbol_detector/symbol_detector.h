 /**************************************************************************//**
 * 
 * @file  symbol_detector.h
 * @brief Header file for the symbol detection kernel.
 * 
 * The symbol detector is designed to read in image frames and output whatever
 * target symbols may be present within.
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
	cv::Mat normalized;
	cv::Mat res;
	cv::Point2f center_norm;
	cv::Point2f top_norm;
	cv::Point2f center;
	cv::Point2f top;
	float add_z;
	cv::Mat affine;
	std::vector<cv::Point>contour;
	std::vector<cv::Point2f>float_contour;
	
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
	SymbolDetector(const Mat &m, const Mat &d);
	~SymbolDetector(void);
	
	void ProcessFrame(const Mat &input, vector<Marker> &output);
	
private:
	Mat int_mat, dist_coeff;
};

#endif