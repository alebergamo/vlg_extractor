/*
 *  prefilt.h
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include <opencv/cv.h>
#include <opencv/highgui.h>

void prefilt(const IplImage* img,IplImage* prefedImg,int pad,IplImage* filter2D);
IplImage* cvGetSubImage(IplImage *image, CvRect roi);
