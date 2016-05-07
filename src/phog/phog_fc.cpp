/*
 *  phog_fc.c
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include "phog_fc.h"
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <opencv/highgui.h>
#include <math.h>
#include <stdio.h>

using namespace std;

int phog_fc(int bin360, int bin180, int L, int highthres, IplImage* imgc, CvMat* phog360, CvMat* phog180 )
// bin360: dimension of phog 2pi
// bin180: dimension of phog pi
// L: number of levels
// highthres: high threshold of canny edge detector
// imgc: input image
// phog360: output feature
// phog180: output feature
{
	cv::Mat imgt(imgc,0);
	cv::Mat grayimg;//(imgt.rows,imgt.cols,CV_8UC1);
	cv::cvtColor(imgt,grayimg,CV_BGR2GRAY);
	/* resize */
	cv::Mat img;
	int sz=0;
	int temp=std::max(grayimg.size().height,grayimg.size().width);
	if(temp>500)
	{
		cv::Size size(grayimg.size().width*500/temp,grayimg.size().height*500/temp);
		cv::resize(grayimg,img,size,0,0,cv::INTER_LINEAR);
	}
	else
	{
		grayimg.copyTo(img);
	}
	cv::Mat edge;//(img.rows,img.cols,CV_8UC1);
	cv::Canny(img,edge,0.4*highthres,highthres,3);
	cv::Mat gradx;
	cv::Mat grady;
	cv::Sobel(img,gradx,CV_32FC1,1,0,3);
	cv::Sobel(img,grady,CV_32FC1,0,1,3);
	gradx = gradx/8;
	grady = grady/8;
	cv::Mat gradx2;
	cv::Mat grady2;
	cv::multiply(gradx,gradx,gradx2);
	cv::multiply(grady,grady,grady2);
	cv::Mat grad2;
	cv::Mat grad;
	cv::add(gradx2,grady2,grad2);
	cv::sqrt(grad2,grad);
	int rows = grad.rows;
	int cols = grad.cols;
	int i,j;
	float* ptrx = (float*) gradx.data;
	float* ptry = (float*) grady.data;
	cv::Mat ori180(img.rows,img.cols,CV_32FC1);
	cv::Mat ori360(img.rows,img.cols,CV_32FC1);
	float* ptr180 = (float*)ori180.data;
	float* ptr360 = (float*)ori360.data;
	const float PI = 3.141593;
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		{
			if((*ptrx) == 0)
				*ptrx = FLT_MIN;
			*ptr180 = (atan((*ptry)/(*ptrx))+PI/2)*180/PI;
			*ptr360 = (atan2(*ptry,*ptrx)+PI)*180/PI;
			ptrx++;ptry++;ptr180++;ptr360++;
		}
	}
	/* binMatrix starts */
	cv::Mat Mbin360_temp,Mbin360,binv360;
	cv::Mat Mbin180_temp,Mbin180,binv180;
	Mbin360_temp = ori360/(360/bin360);
	Mbin180_temp = ori180/(180/bin180);
	vector<int> vec_Mbin360,vec_Mbin180;
	vector<float> vec_binv360,vec_binv180;
	vector<int> pointsrow,pointscol;

	uchar* edgeptr = edge.data;
	float* ptr360bt = (float*) Mbin360_temp.data;
	float* ptr180bt = (float*) Mbin180_temp.data;;
	float* ptrgrad = (float*) grad.data;
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		{
			if((*edgeptr)!=0)
			{
				if((*ptrgrad)!=0)
				{
					vec_Mbin360.push_back(int(ceil(*ptr360bt))-1);vec_Mbin180.push_back(int(ceil(*ptr180bt))-1);///0-19 and 0-39 bug here
					vec_binv360.push_back(*ptrgrad);vec_binv180.push_back(*ptrgrad);
					pointsrow.push_back(i);pointscol.push_back(j);
				}
			}
			edgeptr++;ptr360bt++;ptr180bt++;ptrgrad++;
		}
	}

	/* histogram and normalization */
	/* level 0 */
	temp = pow(float(4),L+1);
	int histlen180 = bin180*((temp-1)/3);
	int histlen360 = bin360*((temp-1)/3);
	cv::Mat* hist180 = new cv::Mat(1,histlen180,CV_32FC1,cv::Scalar(0));
	cv::Mat* hist360 = new cv::Mat(1,histlen360,CV_32FC1,cv::Scalar(0));
	float* ptrhist180 = (float*) hist180->data;
	float* ptrhist360 = (float*) hist360->data;
	int sum180 = 0.0;
	int sum360 = 0.0;
	for(i=0;i<pointsrow.size();i++)
	{
		*(ptrhist180+vec_Mbin180.at(i)) += vec_binv180.at(i);
		sum180 += vec_binv180.at(i);
		*(ptrhist360+vec_Mbin360.at(i)) += vec_binv360.at(i);
		sum360 += vec_binv360.at(i);
	}
	/* level 1--3 */
	vector<int> rowcell,colcell;
	for(i=0;i<L;i++)
	{
		for(j=0;j<pointsrow.size();j++)
		{
			rowcell.push_back(pointsrow.at(j)/(rows/pow((float)2,i+1)));
			colcell.push_back(pointscol.at(j)/(cols/pow((float)2,i+1)));
		}
		int base180 = bin180 * ((int)pow((float)4,i+1)-1)/3;
		int base360 = bin360 * ((int)pow((float)4,i+1)-1)/3;
		int index180,index360;

		for(j=0;j<colcell.size();j++)
		{
			index180 = base180+(pow((float)2,i+1)*colcell.at(j)+rowcell.at(j))*bin180+vec_Mbin180.at(j);
			index360 = base360+(pow((float)2,i+1)*colcell.at(j)+rowcell.at(j))*bin360+vec_Mbin360.at(j);
			*(ptrhist180+index180) += vec_binv180.at(j);
			sum180 += vec_binv180.at(j);
			*(ptrhist360+index360) += vec_binv360.at(j);
			sum360 += vec_binv360.at(j);
		}
		rowcell.clear();
		colcell.clear();
	}
	/* normalization */
	if(sum180!=0)
	{
		for(i=0;i<histlen180;i++)
		{
			*(ptrhist180+i) = *(ptrhist180+i)/sum180;
		}
	}
	else {
		//printf("\nPhog180VectorZero\n");
	}
	if(sum360!=0)
	{
		for(i=0;i<histlen360;i++)
		{
			*(ptrhist360+i) = *(ptrhist360+i)/sum360;
		}
	}
	else {
		//printf("\nPhog360VectorZero\n");
	}
	/* pass the result */
	void* temp360 = phog360->data.fl;
	void* temp180 = phog180->data.fl;
	memcpy(temp180,hist180->data,sizeof(float)*hist180->rows*hist180->cols);
	memcpy(temp360,hist360->data,sizeof(float)*hist360->rows*hist360->cols);
	delete hist180;
	delete hist360;

#if _MSC_VER
	if (_isnan(*((float*)temp180)) || _isnan(*((float*)temp360))) {
#else
	if (isnan(*((float*)temp180)) || isnan(*((float*)temp360))) {
#endif
		return ERROR_PHOG_EXTRACTION_FAILS;
	}

	return 0;

}
