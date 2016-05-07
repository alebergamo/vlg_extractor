/*
 *  padarraySym.c
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */

/* this function can pad the input image symmetrically */
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "padarraySym.h"

void padarraySym(const IplImage* src,IplImage* result, int pad_width)
{
	//IplImage* result=cvCreateImage(cvSize(src->width+2*pad_width,src->height+2*pad_width),src->depth,src->nChannels);
	CvRect a=cvRect(pad_width,pad_width,src->width,src->height);
	cvSetImageROI(result,a);
	cvCopy(src,result);
	cvResetImageROI(result);
	float* ptr=(float*) result->imageData;
	int width_step=result->width;
	int col,row;

	for(col=pad_width-1;col>-1;col--)
	{
		for(row=pad_width;row<(src->height+pad_width);row++)
		{
			*(ptr+row*width_step+col)=*(ptr+row*width_step+pad_width*2-1-col);
		}
	}
	for(col=pad_width+src->width;col<2*pad_width+src->width;col++)
	{
		for(row=pad_width;row<(src->height+pad_width);row++)
		{
			*(ptr+row*width_step+col)=*(ptr+row*width_step+2*(pad_width+src->height)-1-col);
		}
	}

	for(row=pad_width-1;row>-1;row--)
	{
		for(col=pad_width;col<(pad_width+src->width);col++)
		{
			*(ptr+row*width_step+col)=*(ptr+(2*pad_width-row-1)*width_step+col);
		}
	}

	for(col=pad_width;col<(pad_width+src->width);col++)
	{
		for(row=pad_width+src->height;row<2*pad_width+src->height;row++)
		{
			*(ptr+row*width_step+col)=*(ptr+width_step*(2*(pad_width+src->height)-1-row)+col);
		}
	}


	/* pad the four edges */
	for(row=0;row<pad_width;row++)
	{
		for(col=0;col<pad_width;col++)
		{
			*(ptr+row*width_step+col)=*(ptr+(2*pad_width-row-1)*width_step+col);
		}
	}

	for(row=0;row<pad_width;row++)
	{
		for(col=src->width+2*pad_width-1;col>src->width+pad_width-1;col--)
		{
			*(ptr+row*width_step+col)=*(ptr+(2*pad_width-row-1)*width_step+col);
		}
	}
	for(row=pad_width+src->height;row<2*pad_width+src->height;row++)
	{
		for(col=0;col<pad_width;col++)
		{
			*(ptr+row*width_step+col)=*(ptr+row*width_step+2*pad_width-1-col);
		}
	}
	for(row=src->height+2*pad_width-1;row>(pad_width+src->height-1);row--)
	{
		for(col=src->width+2*pad_width-1;col>(pad_width+src->width-1);col--)
		{
			*(ptr+row*width_step+col)=*(ptr+(2*(pad_width+src->height)-1-row)*width_step+col);
		}
	}
}
