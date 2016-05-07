/*
 *  makePrefilter.h
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef MAKEPREFILTER_H
#define MAKEPREFILTER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define ERROR_PREFILTER_READ_PROBLEM -1

#ifdef __cplusplus
extern "C" {
#endif

int makePrefilter(char* filterPath,IplImage** filter2D);

#ifdef __cplusplus
}
#endif

#endif
