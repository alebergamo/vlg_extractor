/*
 *  phog_fc.h
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef PHOG_FC_H
#define PHOG_FC_H

#include <opencv/cv.h>
#include <opencv/cxcore.h>

#define PHOG_LEN360 3400
#define PHOG_LEN180 1700

#define ERROR_PHOG_EXTRACTION_FAILS -1

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  bin360: dimension of phog 2pi
    bin180: dimension of phog pi
    L: number of levels
    highthres: high threshold of canny edge detector
    imgc: input image
    phog360: output feature
    phog180: output feature
 * */
int phog_fc(int bin360, int bin180, int L, int highthres, IplImage* imgc, CvMat* phog360, CvMat* phog180 );


#ifdef __cplusplus
}
#endif


#endif

