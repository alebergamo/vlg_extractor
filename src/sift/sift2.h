/***************************************************************************
 *            sift.h
 *
 *  Fri Dec 17 16:07:01 2010
 *
 *  Copyright  2010  Alessandro Bergamo, Chen Fang
 *  <aleb@cs.dartmouth.edu>
 *  <chen@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef SIFT_H_
#define SIFT_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vlg_toolbox/matrix.h>
#include "siftpp_wrapper.h"
/* parameters to reproduce the pnorm preprocessing pipeline */
#define WPERCENT 0.01
#define BPERCENT 0.01
#define MAXEXPAND 4
#define HISTO_SIZE 10500
#define HISTO_L 2
#define ERROR_SIFT_EXTRACTION_FAIL -1

/*
 * */
int sift_extraction(IplImage* img_original, Matrix* feature, float* centers);


#endif /* SIFT_H_ */
