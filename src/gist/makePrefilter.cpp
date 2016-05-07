/*
 *  makePrefilter.c
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include "makePrefilter.h"

#define PADIMGSIZE 42

int makePrefilter(char* filterPath,IplImage** filter2D)
{
	/* read prefilters from disk */
	FILE* preComp;
	preComp=fopen(filterPath,"r");
	if(!preComp)
		return ERROR_PREFILTER_READ_PROBLEM;
	float* fileData;
	fileData= (float *) malloc((PADIMGSIZE*PADIMGSIZE) * sizeof(float));
	fread(fileData,sizeof(float),PADIMGSIZE*PADIMGSIZE,preComp);
	fclose(preComp);

	IplImage* filter=cvCreateImage(cvSize(PADIMGSIZE,PADIMGSIZE),IPL_DEPTH_32F,1);
	*filter2D=cvCreateImage(cvSize(PADIMGSIZE,PADIMGSIZE),IPL_DEPTH_32F,2);
	/* construct a filter image */
	float* ptr=(float*)filter->imageData;
	int i;
	for(i=0;i<PADIMGSIZE*PADIMGSIZE;i++)
	{
		*(ptr+i)=fileData[i];
	}
	cvMerge(filter,filter,NULL,NULL,*filter2D);
	cvReleaseImage(&filter);
	free(fileData);

	return 0;
}
