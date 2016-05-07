/*
 *  extract_gist.h
 *
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef EXTRACTGIST_H
#define EXTRACTGIST_H

#include "gistFeature.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define GIST_NWINDOWS 4
#define GIST_IMGSIZE 32
#define GIST_NFILTERS 20
#define GIST_CHANNEL 3

#define ERROR_GIST_EXTRACTION_PROBLEM -1

#ifdef __cplusplus
extern "C" {
#endif

int extract_gist(IplImage* img, IplImage* prefilter, IplImage** gabor, CvMat* gistM/*gistFeature* gist*/);

#ifdef __cplusplus
}
#endif

#endif
