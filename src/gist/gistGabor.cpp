/*
 *  gistGabor.c
 *
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include "gistGabor.h"
#include "gistFeature.h"
#include <math.h>

#define NFILTERS 20

void gistGabor(const IplImage* img,int Nwindows,IplImage** gabors2D,struct gistFeature* gistfeature)
{
	int imgSize=img->height;//every img is preprocessed to be square
	int i,j;
	float* ptr=NULL;
	
	/* FFT */
	IplImage* temp[3];
	IplImage* temp1[3];
	IplImage* fftImg[3];
	for(i=0;i<3;i++)
	{
		temp[i]=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,1);
		temp1[i]=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,2);
		fftImg[i]=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,2);
	}
	cvSplit(img,temp[0],temp[1],temp[2],NULL);
	IplImage* merge=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,1);
	cvSetZero(merge);
	for(i=0;i<3;i++)
	{
		cvMerge(temp[i],merge,NULL,NULL,temp1[i]);
		cvFFT(temp1[i],fftImg[i],CV_DXT_FORWARD);
		cvReleaseImage(&temp[i]);
		cvReleaseImage(&temp1[i]);
	}
	cvReleaseImage(&merge);

	IplImage* temp3[NFILTERS*3];
	
	for(j=0;j<3;j++)
	{
		for(i=0;i<NFILTERS;i++)
		{
			temp3[3*i+j]=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,2);
			cvMul(fftImg[j],gabors2D[i],temp3[3*i+j]);
		}
		cvReleaseImage(&fftImg[j]);
		//cvReleaseImage(&gabors2D[i]);
	}

	/* Inverse FFT */
	IplImage* temp4[NFILTERS*3];
	for(i=0;i<NFILTERS*3;i++)
	{
		temp4[i]=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,2);
		cvFFT(temp3[i],temp4[i],CV_DXT_INVERSE_SCALE);
		cvReleaseImage(&temp3[i]);
	}

	/* calculate the abs of the outcomes of IFFT for each channel */
	float absV=0;
	float* ptrAbs=NULL;
	IplImage* temp5[NFILTERS*3];
	float imaginary=0;
	for(i=0;i<NFILTERS*3;i++)
	{
		temp5[i]=cvCreateImage(cvSize(imgSize,imgSize),IPL_DEPTH_32F,1);
		ptr=(float*) temp4[i]->imageData;
		ptrAbs=(float*) temp5[i]->imageData;
		for(j=0;j<imgSize*imgSize;j++)
		{
			imaginary=fabs((*(ptr+1))*(*(ptr+1)));
			absV=sqrt((*ptr)*(*ptr)+imaginary);
			*ptrAbs=absV;
			ptrAbs++;
			ptr++;ptr++;
		}
		cvReleaseImage(&temp4[i]);
	}
	/*calculate the gist features according to different windows*/
	int x=0;
	int y=0;
	int width=0;
	int height=0;
	int stepWidth=imgSize/Nwindows;
	int k;
	CvScalar s = cvScalar(0,0,0,0);
	for(k=0;k<NFILTERS*3;k++)
	{
		for(i=0;i<Nwindows;i++)
		{
			for(j=0;j<Nwindows;j++)
			{
				x=j*stepWidth;
				y=i*stepWidth;
				width=stepWidth;
				height=stepWidth;
				if(j==Nwindows-1)
					width=width+imgSize%Nwindows;
				if(i==Nwindows-1)
					height=width+imgSize%Nwindows;
				cvSetImageROI(temp5[k],cvRect(x,y,width,height));
				s=cvAvg(temp5[k]);
				(gistfeature->features[k*Nwindows*Nwindows+i*Nwindows+j])=s.val[0];
			}
		}
	}
	for(i=0;i<NFILTERS*3;i++)
	{
		cvReleaseImage(&temp5[i]);
		//cvReleaseImage(&fftImg[j]);
	}
}
