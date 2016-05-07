/*
 *  makeGabor.c
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "makeGabor.h"
#include <stdio.h>


#define IMGSIZE 32
#define NFILTERS 20

int makeGabor(char* gaborPath,IplImage** gabors2D)
{
	/* read the precomputed gabor filters from file */
	FILE* file;
	file=fopen(gaborPath,"r");
	if(!file)
		return ERROR_GABOR_READ_PROBLEM;
	float* fileData=(float*) malloc(sizeof(float)*IMGSIZE*IMGSIZE*NFILTERS);
	fread(fileData,sizeof(float),IMGSIZE*IMGSIZE*NFILTERS,file);
	fclose(file);

	/* construct a series of of different gabor filters */
	IplImage* gabors1D[NFILTERS];
	int i,j;
	float* ptr=NULL;

	for(i=0;i<NFILTERS;i++)
	{
		gabors1D[i]=cvCreateImage(cvSize(IMGSIZE,IMGSIZE),IPL_DEPTH_32F,1);
		gabors2D[i]=cvCreateImage(cvSize(IMGSIZE,IMGSIZE),IPL_DEPTH_32F,2);
		ptr=(float*)gabors1D[i]->imageData;
		for(j=0;j<IMGSIZE*IMGSIZE;j++)
		{
			*ptr=fileData[i*IMGSIZE*IMGSIZE+j];
			ptr++;
		}
		cvMerge(gabors1D[i],gabors1D[i],NULL,NULL,gabors2D[i]);
		cvReleaseImage(&gabors1D[i]);
	}
	free(fileData);

	return 0;

}
