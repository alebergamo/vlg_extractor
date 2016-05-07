/*
 *  makeGabor.h
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef MAKEGABOR_H
#define MAKEGABOR_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define ERROR_GABOR_READ_PROBLEM -1


#ifdef __cplusplus
extern "C" {
#endif

int makeGabor(char* path,IplImage** gabor);

#ifdef __cplusplus
}
#endif

#endif
