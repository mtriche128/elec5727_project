/**************************************************************************//**
 * 
 * @file  symbol_detector.cpp
 * @brief The symbol detection kernel.
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
#include <opencv2/gpu/gpu.hpp>
#include "symbol_detector_GPU.h"
#include "features.h"
using namespace std;
using namespace cv;

/* ------------------------------------------------------------------------- *
 * Define Internal Functions                                                 *
 * ------------------------------------------------------------------------- */

int id_petal(cv::Mat &input_image, cv::Point p) 
{
	const unsigned char * data = input_image.data;
	float r = data[(p.x+input_image.cols*p.y)];
	
	if(r>100)return 0;
	return 1;
}

/* ------------------------------------------------------------------------- *
 * Define Exernal Methods                                                    *
 * ------------------------------------------------------------------------- */

void Marker::calc_number(void) {
	number=-1;
	for(int i=0;i<sizeof(petal_patts)/sizeof(Pattern);++i){
		for(int r=0;r<8;++r){
			bool match =true;
			for(int p=0;p<8;++p){
				if(petal_vals[p]!=(petal_patts[i].data[(p+r*2)%8]?0:1)){
					match=false;
					break;
				}
			}
			if(match){
				number=petal_patts[i].id;
				if(r==0)top_norm=cv::Point2f(0.9,0.5);
				if(r==1)top_norm=cv::Point2f(0.5,0.9);
				if(r==2)top_norm=cv::Point2f(0.1,0.5);
				if(r==3)top_norm=cv::Point2f(0.5,0.1);
				
				add_z = r*3.14159265359/2.;
				break;
			}
		}
	}
	
}

/**
 * @brief Destructor.
 */

SymbolDetectorGPU::~SymbolDetectorGPU(void)
{
	// Do nothing for now.
}

/**
 * @brief Push an image frame into the symbol detector.
 * 
 * @param[in]  input  The image frame.
 * @param[out] output The list of detected symbols.
 */
void SymbolDetectorGPU::push(const Mat &input, vector<Marker> &output)
{
	// Initialize first available GPU
	//cv::gpu::setDevice(0);
	
	int marker_size = 48;
	//cv::Mat im_res = input.clone();
	
	//--------GPU Start----------
	// upload image to GPU
	cv::gpu::GpuMat gpu_input;
	gpu_input.upload(input);
	
	/*cv::Mat im_gray;*/
	cv::gpu::GpuMat im_gray;
	
	/*cvtColor(input,im_gray,CV_RGB2GRAY);*/
	cv::gpu::cvtColor(gpu_input,im_gray,CV_RGB2GRAY);
		
	/*cv::normalize(im_gray,im_gray,0,255,cv::NORM_MINMAX, CV_8UC3);*/
	cv::gpu::normalize(im_gray,im_gray,0,255,cv::NORM_MINMAX, CV_8UC3);
	
	/*cv::Mat im_thresh;*/
	cv::gpu::GpuMat gpu_im_thresh;
	cv::gpu::threshold(im_gray,gpu_im_thresh,128,255,cv::THRESH_BINARY);

	cout << "A" << endl;
	// BOUNDRY: out_ims = im_thresh;
	// ----------------------------------------------------------------------
	// Find Contours
	
	// Pull off the CPU
	cv::Mat im_thresh;
	gpu_im_thresh.download(im_thresh);
	//--------GPU End----------	
	
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(im_thresh.clone(), contours, CV_RETR_LIST,CV_CHAIN_APPROX_TC89_L1);

	cout << "B" << endl;	
	// BOUNDRY:
	// ----------------------------------------------------------------------
	// Process Contours
	
	//out_ims = input_image;
	
	std::vector<cv::Point> approx;
	std::vector<Marker> markers;
	//TODO: GPU Accelerator candidate
	for (int i = 0; i < contours.size(); i++){
		cout << "start contour for" << endl;	
		cv::approxPolyDP(
			cv::Mat(contours[i]),
				     approx,
			 cv::arcLength(cv::Mat(contours[i]), true) * 0.03 ,
				     true
		);
		
		if (std::fabs(cv::contourArea(approx)) < 30 )
			continue;
		if(approx.size()!=4)continue;
		std::vector<cv::Point2f> square_match(4);
		square_match[0] = cv::Point2f(0,0);
		square_match[1] = cv::Point2f(0,marker_size );
		square_match[2] = cv::Point2f(marker_size ,marker_size );
		square_match[3] = cv::Point2f(marker_size ,0);
	
		std::vector<cv::Point2f> square_to;
		square_to.resize(4);
		cv::Size winSize( 7, 7);
		cv::Size zeroZone( -1, -1 );
		cout << "start term crit" << endl;	
		cv::TermCriteria criteria = cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 50, 0.0001 );
		cout << "end term crit" << endl;	
		//TODO: Definite mulicore Accelerator candidate
		for(int i=0;i<4;++i){square_to[i]=approx[i];}
		/// Calculate the refined corner locations
		cout << "start subpix" << endl;	
		cv::cornerSubPix( im_gray, square_to, winSize, zeroZone, criteria );
		cout << "end subpix" << endl;	
		cout << "start perspective trans" << endl;	
		cv::Mat affine = cv::getPerspectiveTransform(square_to,square_match);
		cout << "end perspective trans" << endl;
		cout << "start GPU section 1" << endl;		
		//--------GPU Start----------
		// upload to GPU
		cv::gpu::GpuMat gpu_normalized;
		
		/*cv::warpPerspective(input,normalized,affine,cv::Size2f(marker_size ,marker_size ));*/
		cv::gpu::warpPerspective(gpu_input,gpu_normalized,affine,cv::Size2f(marker_size ,marker_size ));
		
		cout << "   * normalize...";
		cv::gpu::normalize(gpu_normalized,gpu_normalized,0,255,cv::NORM_MINMAX, CV_8UC3);
		cout << "done" << endl;		
	
		// download from GPU
		cv::Mat normalized;
		gpu_normalized.download(normalized);
		//--------GPU End----------	
		cout << "end GPU section 1" << endl;	
		
		Marker m;
		m.normalized=normalized;
		m.affine = affine;
		m.contour = approx;
		m.float_contour=square_to;
		markers.push_back(m);
	
		cout << "end contour for" << endl;	
		/* NOTE: Each markers[i].contour shall contain 4 points which are
		 *       the corners of the detected markers within the frame.
		 */
		
		/* Example Output
		for(int c = 0; c < markers.size(); c++)
		{
			for(int p = 0; p < markers[c].contour.size(); p++)
			{
				circle(out_ims, markers[c].contour[p], 5, Scalar(0,255,0), 1);
			}
			
		}
		*/
	}

	cout << "C" << endl;	
	// ----------------------------------------------------------------------
	// Process Markers
	
	std::vector<Marker> next_markers;
	
	/* NOTE: Each markers[i].normalized shall contain a small normalized
	 *       "cut-out" of a suspected marker taken from the frame.
	 */
	/* Example Output
	if(markers.size() >= 3)
	{
		cv::Mat mark_gray;		
		cvtColor(markers[2].normalized,mark_gray,CV_RGB2GRAY);
		out_ims = mark_gray;
	}
	*/

	for(auto&m : markers){
		m.res=m.normalized;
		std::vector<cv::Point2f> corners;
		cv::Mat corners_mat;
		cv::Mat mark_gray;
		
		//--------GPU Start----------
		// upload to GPU
		//normalized already on GPU
		cv::gpu::GpuMat gpu_mark_gray;
		gpu_mark_gray.upload(mark_gray);
		cv::gpu::GpuMat gpu_normalized;
		gpu_normalized.upload(m.res);
		
		cv::gpu::GpuMat gpu_corners;
		gpu_corners.upload(corners_mat);
		
		//cvtColor(m.normalized,mark_gray,CV_RGB2GRAY);
		//cv::goodFeaturesToTrack(mark_gray,corners,4,0.03,10);
		cv::gpu::cvtColor(gpu_normalized,gpu_mark_gray,CV_RGB2GRAY);
		//cv::gpu::GoodFeaturesToTrackDetector_GPU(gpu_mark_gray,gpu_corners,4,0.03,10);
		
		cout << "start TROUBLE" << endl;
		// Mask not defined. Potential problem area. Check back here if you get poor results. 
		cv::gpu::GoodFeaturesToTrackDetector_GPU corner_detect(4,0.03,10);
		corner_detect(gpu_mark_gray,gpu_corners);

		cout << "end TROUBLE" << endl;
		// download from GPU
		gpu_corners.download(corners_mat);
		gpu_mark_gray.download(mark_gray);
		
		corners = corners_mat;
		//--------GPU End----------	
		
		if(corners.size()==0)continue;
		/// Set the needed parameters to find the refined corners
		cv::Size winSize( 1, 1 );
		cv::Size zeroZone( -1, -1 );
		cv::TermCriteria criteria = cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );
		
		/// Calculate the refined corner locations
		cv::cornerSubPix( mark_gray, corners, winSize, zeroZone, criteria );
		double best_error =1e100;
		cv::Point2f center;
		
		//TODO: Multicore Accelerate
		for(int i=0;i<corners.size();++i){
			double error = fabs(corners[i].x-marker_size*0.5)+fabs(corners[i].y-marker_size*0.5);
			if(error<best_error){
				center = corners[i];
				best_error = error;
			}
		}
		if(best_error/marker_size>0.1)continue;
		cv::circle(m.res,center,1,cv::Scalar(0,255,0),1);
		m.center_norm =cv::Point(marker_size*0.5,marker_size*0.5);
		
		cv::Mat aff_inv = m.affine.inv();
		std::vector<cv::Point2f> in_c={m.center_norm};
		std::vector<cv::Point2f> out_c;
		
		cv::perspectiveTransform(in_c,out_c,aff_inv);
		m.center=out_c[0];
		
		std::vector<cv::Point> petal_locs;
		
		
		for(int i=0;i<8;++i){
			petal_locs.push_back(
				cv::Point(
					marker_size*0.5-cos((i)/8.*2.*3.141592)*marker_size*0.25,
					    marker_size*0.5+sin((i)/8.*2.*3.141592)*marker_size*0.25
				));
		}
		
		//TODO: Multicore Accelerator candidate
		for(int p=0;p<petal_locs.size();++p){
			int type = id_petal(mark_gray,petal_locs[p]);
			m.petal_vals[p]=type;
			cv::Scalar s(255,255,255);
			if(type==0)s=cv::Scalar(0,255,0);
			cv::circle(m.res,petal_locs[p],1,s,1);
		}
		m.number=0;
		std::vector<cv::Point> petal_negs=
		{
			cv::Point( m.center_norm.x+0.38*marker_size,m.center_norm.y+0.38*marker_size),
			cv::Point( m.center_norm.x-0.38*marker_size,m.center_norm.y+0.38*marker_size),
			cv::Point( m.center_norm.x-0.38*marker_size,m.center_norm.y-0.38*marker_size),
			cv::Point( m.center_norm.x+0.38*marker_size,m.center_norm.y-0.38*marker_size),
			
		};
		for(int p=0;p<petal_negs.size();++p){
			int type = id_petal(mark_gray,petal_negs[p]);
			if(type==0){
				m.number=-1;
				break;
			}
			
			cv::circle(m.res,petal_negs[p],1,cv::Scalar(255,0,0),1);
			
		}
		if(m.number==-1)break;
		m.calc_number();
		
		if(m.number!=-1)
		{
			std::vector<cv::Point2f> in_c={cv::Point(m.top_norm.x*marker_size,m.top_norm.y*marker_size)};
			std::vector<cv::Point2f> out_c;
			
			cv::perspectiveTransform(in_c,out_c,aff_inv);
			m.top=out_c[0];
			
			next_markers.push_back(m);
		}
	}

	cout << "D" << endl;
	
	// Example Output
	// if(markers.size() >= 3)
	// {
	// 	if((markers[2].res.cols*markers[2].res.rows) > 0)
	// 		out_ims = markers[2].res;
	// }

	markers=next_markers;
	
	if(markers.size()){
		
		int y = (markers.size()-1)/3+1;
		double marker_width = 1.9685;
		
		cv::Mat marker_res(y*marker_size ,3*marker_size ,CV_8UC4);
		
		for(int i=0;i<markers.size();++i){
			
			int xp = i%3;
			int yp = i/3;
			double cosz =marker_width*cos(markers[i].add_z)/2;
			double sinz =marker_width*sin(markers[i].add_z)/2;
			
			std::vector<cv::Point3f> object_points={
				cv::Point3f(-cosz-sinz,+cosz+sinz,0),
				cv::Point3f(-cosz+sinz,-cosz+sinz,0),
				cv::Point3f(cosz+sinz,-cosz-sinz,0),
				cv::Point3f(cosz-sinz,+cosz-sinz,0),
			};
			
			output = markers;
		}
	}

	cout << "E" << endl;
}
