/*
 *  hist_ssim.c
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include <opencv/cv.h>
#include <opencv/cv.hpp>
//#include <cvaux.h>
//#include <cvaux.hpp>
#include <cxcore.h>
#include <cxcore.hpp>
#include <highgui.h>
#include "SelfSimDescriptor_fc.h"
#include <iostream>
#include <time.h>
#include "quantize.h"
#include "hist_ssim.h"

#define CDIM 30
#define NCENTERS 300
#define MASKSIZE 81*81

using namespace std;

int mapreader(int** maskarray,char* path)
/* read the logpoloar map from disk */
{
	FILE* mappingfile=fopen(path,"rb");
	if(!mappingfile)
		return ERROR_MAP_READ_PROBLEM;
	*maskarray=(int*) malloc(sizeof(int)*MASKSIZE);
	fread(*maskarray,sizeof(int),MASKSIZE,mappingfile);
	fclose(mappingfile);
	return 0;
}

int centereader( char* path, float* centers)
/* read ssim cluster centers from disk */
{
	FILE* centerFile=fopen(path,"rb");
	if(!centerFile)
		return ERROR_CENTER_READ_PROBLEM;
	fread(centers,sizeof(float),CDIM*NCENTERS,centerFile);
	fclose(centerFile);
	return 0;
}

int hist_ssim(IplImage* imgipl, float* centers,int* logpolar, CvMat* histMat, CvMat** local_descriptor)
// imgipl: input image
// centers: a float array of kmeans centers
//        if centers==NULL we do *NOT* quantize; instead 'histMat' will be the the local descriptor
// logpolar: an integer array of logpolar mapping
// histMat: output feature
{
		Mat imgMat(imgipl,0);
		Mat resizedMat;
		SelfSimDescriptor_fc test(5,81,0,3,10);
		std::vector<float> descriptors;
		vector<int> pointsX;
		vector<int> pointsY;
		/* resize the image */
		int sz=0;
		int temp=std::max(imgMat.size().height,imgMat.size().width);
		if(temp>500)
		{
			cv::Size size(imgMat.size().width*500/temp,imgMat.size().height*500/temp);
			cv::resize(imgMat,resizedMat,size,0,0,cv::INTER_LINEAR);
		}
		else
		{
			imgMat.copyTo(resizedMat);
		}
		Size winStride(5,5);
		test.compute(resizedMat,descriptors,pointsX, pointsY, logpolar, winStride);

		if (centers == NULL) {
			// return the local descriptor
			*local_descriptor = cvCreateMat(descriptors.size(), 1, CV_32SC1);
			for (unsigned int i=0; i < descriptors.size(); ++i) {
				cvSetReal2D(*local_descriptor, i, 0, descriptors[i]);
			}
			//TODO. this line is much more efficient, but it does not work. memcpy((*local_descriptor)->data.fl, &descriptors[0], descriptors.size()*sizeof(float));
		} else {
			// quantize
			int* hist=NULL;
			hist=(int*) malloc(sizeof(int)*NCHANNEL);
			int histlen=quantize(descriptors,pointsX,pointsY,hist,centers,resizedMat.size().height,resizedMat.size().width);
			memcpy(histMat->data.i, hist, sizeof(int)*histMat->rows*histMat->cols);
			if(hist==NULL)
				return ERROR_SSIM_HIST_EXTRACTION_PROBLEM;
			free(hist);
		}


		return 0;
}

