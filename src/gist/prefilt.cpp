/*
 *  prefilt.c
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include "prefilt.h"
#include "padarraySym.h"

void prefilt(const IplImage* img,IplImage* prefedImg,int pad,IplImage* filter2D)
{
	int imagesize=img->height;
	int padImgSize=imagesize+2*pad;
	
	IplImage* temp1=cvCreateImage(cvSize(imagesize,imagesize),IPL_DEPTH_32F,3);
	
	/*put 8 bits image into 32 bits float image*/
	cvConvertScale(img,temp1,1,1);

	int i;
	//use ptr to get access to data blocks/pixels,each block/pixel has three float numbers
	IplImage* temp2=cvCreateImage(cvSize(imagesize,imagesize),IPL_DEPTH_32F,3);
	float* ptr=NULL;

	cvLog(temp1,temp2);
	cvReleaseImage(&temp1);

	/*pad the image to reduce boundary artifacts*/
	IplImage* temp5=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,3);
	IplImage* pad1=cvCreateImage(cvSize(imagesize,imagesize),IPL_DEPTH_32F,1);
	IplImage* pad2=cvCreateImage(cvSize(imagesize,imagesize),IPL_DEPTH_32F,1);
	IplImage* pad3=cvCreateImage(cvSize(imagesize,imagesize),IPL_DEPTH_32F,1);
	cvSplit(temp2,pad1,pad2,pad3,NULL);
	IplImage* C1=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
	IplImage* C2=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
	IplImage* C3=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
	padarraySym(pad1,C1,pad);/* pad each channel */
	padarraySym(pad2,C2,pad);
	padarraySym(pad3,C3,pad);
	cvMerge(C1,C2,C3,NULL,temp5);

	cvReleaseImage(&pad1);
	cvReleaseImage(&pad2);
	cvReleaseImage(&pad3);
	cvReleaseImage(&C1);
	cvReleaseImage(&C2);
	cvReleaseImage(&C3);



	/* FFT */
	/* split 3-channel image to single channel images */
	IplImage* splitImgs[3];
	IplImage* dftOutput[3];
	IplImage* splitImgsC2[3];
	for(i=0;i<3;i++)
	{
		splitImgs[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
		splitImgsC2[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
		dftOutput[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
		cvSetZero(splitImgs[i]);
		cvSetZero(splitImgsC2[i]);
		cvSetZero(dftOutput[i]);
	}
	cvSplit(temp5,splitImgs[0],splitImgs[1],splitImgs[2],NULL);
	IplImage* mergSrc=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
	cvSetZero(mergSrc);
	/* merge to double channel images for input of FFT */
	for(i=0;i<3;i++)
	{
		cvMerge(splitImgs[i],mergSrc,NULL,NULL,splitImgsC2[i]);
		cvReleaseImage(&splitImgs[i]);
	}
	/* FFT */
	for(i=0;i<3;i++)
	{
		cvDFT(splitImgsC2[i],dftOutput[i],CV_DXT_FORWARD);
		cvReleaseImage(&splitImgsC2[i]);
	}

	/* apply prefilt to three channels */
	IplImage* output_temp[3];
	IplImage* outputReal[3];
	IplImage* outputImaginary[3];
	IplImage* ifftOutput[3];
	float temp=0;
	int j=0;
	float* ptr1=NULL;
	for(i=0;i<3;i++)
	{
		output_temp[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
		cvSetZero(output_temp[i]);
		cvMul(filter2D,dftOutput[i],output_temp[i]);
		cvReleaseImage(&dftOutput[i]);

		outputReal[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
		cvSetZero(outputReal[i]);
		outputImaginary[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
		cvSetZero(outputImaginary[i]);
		ifftOutput[i]=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
		cvSetZero(ifftOutput[i]);
		cvDFT(output_temp[i],ifftOutput[i],CV_DXT_INVERSE_SCALE);
		cvSplit(ifftOutput[i],outputReal[i],outputImaginary[i],NULL,NULL);
		cvReleaseImage(&outputImaginary[i]);
		cvReleaseImage(&ifftOutput[i]);
	}

	/* merge real parts to a 3-channel image */
	IplImage* temp3=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,3);
	cvMerge(outputReal[0],outputReal[1],outputReal[2],NULL,temp3);
	cvReleaseImage(&outputReal[0]);
	cvReleaseImage(&outputReal[1]);
	cvReleaseImage(&outputReal[2]);

	IplImage* prefiltOutput=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,3);
	cvSub(temp5,temp3,prefiltOutput);
	cvReleaseImage(&temp3);
	cvReleaseImage(&temp5);



	/* local contrast normalization */
	/* mean of values in different channels of each pixel */
	IplImage* mean=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
	IplImage* mean2D=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
	ptr=(float*) mean->imageData;
	ptr1=(float*) prefiltOutput->imageData;
	for(i=0;i<(padImgSize)*(padImgSize)*3;i++)
	{
		if((i+1)%3==0)
		{
			temp=(*(ptr1+i)+*(ptr1+i-1)+*(ptr1+i-2))/3;
			*(ptr+(i+1)/3-1)=temp*temp;
		}

	}


	/* FFT */
	IplImage* fftRes=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
	cvMerge(mean,mergSrc,NULL,NULL,mean2D);
	cvReleaseImage(&mergSrc);
	cvDFT(mean2D,fftRes,CV_DXT_FORWARD);
	cvReleaseImage(&mean);
	IplImage* temp4=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
	cvMul(filter2D,fftRes,temp4);

	/* Inverse FFT */
	IplImage* ifftRes=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,2);
	IplImage* lccstTemp=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,1);
	ptr1=(float*) lccstTemp->imageData;
	cvFFT(temp4,ifftRes,CV_DXT_INVERSE_SCALE);
	/* caculate the abs of complex number matrix */
	ptr=(float*)ifftRes->imageData;
	float realV=0;
	float imgV=0;
	for(i=0;i<(padImgSize)*(padImgSize);i++)
	{
		realV=*(ptr+2*i);
		imgV=*(ptr+2*i+1);
		*(ptr1+i)=sqrt(sqrt(realV*realV+fabs(imgV*imgV)))+0.2;
	}
	IplImage* lccst=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,3);

	cvMerge(lccstTemp,lccstTemp,lccstTemp,NULL,lccst);
	IplImage* outputTemp=cvCreateImage(cvSize(padImgSize,padImgSize),IPL_DEPTH_32F,3);
	cvDiv(prefiltOutput,lccst,outputTemp);

	cvReleaseImage(&mean2D);
	cvReleaseImage(&fftRes);
	cvReleaseImage(&ifftRes);
	cvReleaseImage(&lccstTemp);
	cvReleaseImage(&lccst);
	cvReleaseImage(&prefiltOutput);
	cvReleaseImage(&temp2);
	cvReleaseImage(&temp4);
	cvReleaseImage(&output_temp[0]);
	cvReleaseImage(&output_temp[1]);
	cvReleaseImage(&output_temp[2]);


	IplImage* temp6 = cvGetSubImage(outputTemp,cvRect(pad,pad,imagesize,imagesize));
	cvCopy(temp6,prefedImg);
	cvReleaseImage(&outputTemp);
	cvReleaseImage(&temp6);
}


IplImage* cvGetSubImage(IplImage *image, CvRect roi)
{
    IplImage *result;
    // set ROI
    cvSetImageROI(image,roi);
    // create subimage
    result = cvCreateImage( cvSize(roi.width, roi.height), image->depth, image->nChannels );
    cvCopy(image,result);
    cvResetImageROI(image);
    return result;
}
