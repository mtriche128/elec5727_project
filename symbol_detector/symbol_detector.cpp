/**************************************************************************//**
 * Matthew Triche
 * ELEC5727
 * Final Project
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
#include "symbol_detector.h"
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

SymbolDetector::~SymbolDetector(void)
{
	// Do nothing for now.
}

/**
 * @brief Push an image frame into the symbol detector.
 * 
 * @param[in]  input  The image frame.
 * @param[out] output The list of detected symbols.
 */

void SymbolDetector::push(const Mat &input, vector<Marker> &output)
{
	int marker_size = 48;
	//cv::Mat im_res = input.clone();
	
	cv::Mat im_gray;
	cvtColor(input,im_gray,CV_RGB2GRAY);
	cv::normalize(im_gray,im_gray,0,255,cv::NORM_MINMAX, CV_8UC3);
	
	cv::Mat im_thresh;
	cv::threshold(im_gray,im_thresh,128,255,cv::THRESH_BINARY);
	
	// BOUNDRY: out_ims = im_thresh;
	// ----------------------------------------------------------------------
	// Find Contours
	
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(im_thresh.clone(), contours, CV_RETR_LIST,CV_CHAIN_APPROX_TC89_L1);
	
	// BOUNDRY:
	// ----------------------------------------------------------------------
	// Process Contours
	
	//out_ims = input_image;
	
	std::vector<cv::Point> approx;
	std::vector<Marker> markers;
	
	for (int i = 0; i < contours.size(); i++){
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
		cv::TermCriteria criteria = cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 50, 0.0001 );
		
		for(int i=0;i<4;++i){square_to[i]=approx[i];}
		/// Calculate the refined corner locations
		cv::cornerSubPix( im_gray, square_to, winSize, zeroZone, criteria );
		
		cv::Mat affine = cv::getPerspectiveTransform(square_to,square_match);
		cv::Mat normalized;
		cv::warpPerspective(input,normalized,affine,cv::Size2f(marker_size ,marker_size ));
		cv::normalize(normalized,normalized,0,255,cv::NORM_MINMAX, CV_8UC3);
		Marker m;
		m.normalized=normalized;
		m.affine = affine;
		m.contour = approx;
		m.float_contour=square_to;
		markers.push_back(m);
		
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
		cv::Mat mark_gray;
		
		cvtColor(m.normalized,mark_gray,CV_RGB2GRAY);
		cv::goodFeaturesToTrack(mark_gray,corners,4,0.03,10);
		if(corners.size()==0)continue;
		/// Set the needed parameters to find the refined corners
		cv::Size winSize( 1, 1 );
		cv::Size zeroZone( -1, -1 );
		cv::TermCriteria criteria = cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );
		
		/// Calculate the refined corner locations
		cv::cornerSubPix( mark_gray, corners, winSize, zeroZone, criteria );
		double best_error =1e100;
		cv::Point2f center;
		
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

			/* 
			//markers[i].res.copyTo(marker_res(cv::Rect(xp*marker_size ,yp*marker_size ,marker_size ,marker_size )));
			marker_res(cv::Rect(xp*marker_size ,yp*marker_size ,marker_size ,marker_size )) = markers[i].res.clone();
			
			std::vector<cv::Point> approx = markers[i].contour;
			for(int i=0;i<approx.size()-1;++i){
				cv::line(im_res,approx[i],approx[i+1],cv::Scalar(0,0,255),3);
			}

			cv::line(im_res,approx[0],approx[approx.size()-1],cv::Scalar(0,0,255),3);
			cv::circle(im_res,markers[i].center,2,cv::Scalar(0,255,0),1);
			cv::line(im_res,markers[i].center,markers[i].top,cv::Scalar(0,255,0),3);
			
			cv::line(im_res,approx[0],approx[2],cv::Scalar(0,255,255),1);
			cv::line(im_res,approx[1],approx[3],cv::Scalar(0,255,255),1);
			
			std::stringstream str;
			str.precision(4);
			
			cv::Mat rvec,tvec;
			std::vector<cv::Point2f> pts =markers[i].float_contour;
			cv::solvePnP(object_points,pts,m1,d1,rvec,tvec,false,CV_P3P);
			
			str<<markers[i].number;
			cv::putText(im_res,str.str(),markers[i].center,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),markers[i].center,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<tvec.at<double>(0);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+20),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+20),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<tvec.at<double>(1);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+40),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+40),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<tvec.at<double>(2);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+60),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+60),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<rvec.at<double>(0)*180/(3.14159265359);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+80),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+80),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<rvec.at<double>(1)*180/(3.14159265359);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+100),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+100),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<rvec.at<double>(2)*180/(3.14159265359);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+120),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(im_res,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+120),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			*/
		}
	}
}