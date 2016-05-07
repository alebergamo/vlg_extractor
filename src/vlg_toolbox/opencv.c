/*
 *  opencv.c
 *  Xcode_vlg_toolbox
 *
 *  Created by Alessandro Bergamo on 2/11/12.
 *  Copyright 2012 Dartmouth College. All rights reserved.
 *
 */

#include "opencv.h"


Matrix* cvMat_2_Matrix(CvMat* cvM)
{
	Matrix* M_res = NULL;
	int i;
	int j;
	
	assert(cvM);
	
	/* create the Matrix*/
	M_res = (Matrix*) malloc(sizeof(Matrix));
	create_matrix(M_res, cvM->rows, cvM->cols);
	
	/* copy the data */
	for (i=0; i < cvM->rows; ++i) {
		for (j=0; j < cvM->cols; ++j) {
			matrix_set(M_res, i, j, (float) cvGetReal2D(cvM, i, j));
		}
	}
	
	/* return */
	return M_res;
}
