#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "symbol_detector/symbol_detector.h"
using namespace std;
using namespace cv;

int main(void)
{
	SymbolDetector sym_det;
	
	Mat m1 = Mat::eye(3,3,CV_64F);   // intrinsic matrix
	Mat d1 = Mat::zeros(5,1,CV_64F); // distortion coefficients
	
	cout << "===== Symbol Detector =====" << endl;
	
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout << "Error: Unable to open video capture device." << endl;
		return -1;
	}
	
	namedWindow("output",1);
	
	while(1)
	{
		Mat frame;
		vector<Marker> markers;
		cap >> frame;
		
		sym_det.push(frame, markers);
		
		int y = (markers.size()-1)/3+1;
		int marker_size =48;
		double marker_width = 1.9685;
		cv::Mat marker_res(y*marker_size ,3*marker_size ,CV_8UC4);
		
		for(int i=0;i<markers.size();++i)
		{
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
			
			//markers[i].res.copyTo(marker_res(cv::Rect(xp*marker_size ,yp*marker_size ,marker_size ,marker_size )));
			marker_res(cv::Rect(xp*marker_size ,yp*marker_size ,marker_size ,marker_size )) = markers[i].res.clone();
			
			std::vector<cv::Point> approx = markers[i].contour;
			for(int i=0;i<approx.size()-1;++i){
				cv::line(frame,approx[i],approx[i+1],cv::Scalar(0,0,255),3);
			}
			
			cv::line(frame,approx[0],approx[approx.size()-1],cv::Scalar(0,0,255),3);
			cv::circle(frame,markers[i].center,2,cv::Scalar(0,255,0),1);
			cv::line(frame,markers[i].center,markers[i].top,cv::Scalar(0,255,0),3);
			
			cv::line(frame,approx[0],approx[2],cv::Scalar(0,255,255),1);
			cv::line(frame,approx[1],approx[3],cv::Scalar(0,255,255),1);
			
			std::stringstream str;
			str.precision(4);
			
			cv::Mat rvec,tvec;
			std::vector<cv::Point2f> pts =markers[i].float_contour;
			cv::solvePnP(object_points,pts,m1,d1,rvec,tvec,false,CV_P3P);
			
			str<<"["<<markers[i].number<<"]";
			cv::putText(frame,str.str(),markers[i].center,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),markers[i].center,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<tvec.at<double>(0);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+20),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+20),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<tvec.at<double>(1);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+40),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+40),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<tvec.at<double>(2);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+60),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+60),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<rvec.at<double>(0)*180/(3.14159265359);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+80),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+80),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<rvec.at<double>(1)*180/(3.14159265359);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+100),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+100),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
			
			str.str("");
			str<<rvec.at<double>(2)*180/(3.14159265359);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+120),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),3);
			cv::putText(frame,str.str(),cv::Point(markers[i].center.x,markers[i].center.y+120),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,100,0),2);
		}
		
		imshow("output", frame);
		if(waitKey(1) >= 0) break;
	}
	
	
	return 0;
}