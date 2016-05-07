/*
 *  openopencv/cv.h
 *  Xcode_vlg_toolbox
 *
 *  Created by Alessandro Bergamo on 2/11/12.
 *  Copyright 2012 Dartmouth College. All rights reserved.
 *
 */

#ifndef OPENCV_H
#define OPENCV_H

#include "matrix.h"
#include "utils.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	
/* given a matrix CvMat, it converts it in a Matrix type.
 * A new Matrix will be allocated on the heap and the pointer will be returned.
 * */
Matrix* cvMat_2_Matrix(CvMat* cvM);

		
	
#ifdef __cplusplus
}
#endif

#endif
