/*
 *  hist_ssim.h
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef HIST_SSIM_H
#define HIST_SSIM_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define SSIM_CDIM 30
#define SSIM_NCENTERS 300

#define ERROR_MAP_READ_PROBLEM -1
#define ERROR_CENTER_READ_PROBLEM -2
#define ERROR_SSIM_HIST_EXTRACTION_PROBLEM -3

#ifdef __cplusplus
extern "C" {
#endif

int mapreader(int** maskarray,char* path);

int centereader( char* path, float* centers);

// if centers==NULL we do *NOT* quantize; instead 'histMat' will be the the local descriptor
int hist_ssim(IplImage* imgipl, float* centers,int* logpolar, CvMat* histMat, CvMat** local_descriptor);


#ifdef __cplusplus
}
#endif


#endif
