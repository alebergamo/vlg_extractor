/*
 *  matlab.c
 *  Xcode_vlg_toolbox
 *
 *  Created by Alessandro Bergamo on 2/11/12.
 *  Copyright 2012 Dartmouth College. All rights reserved.
 *
 */

#include "matlab.h"

#ifdef MAT_SUPPORT


#include <mat.h>


int read_mat_matrix(Matrix* M, const char* file_name, const char* varname)
{
	MATFile* matf = NULL;
	mxArray* Mx = NULL;
	int res;
	unsigned int i,j;
	double* data;
	
	assert(file_name);
	assert(M);
	assert(varname);
	
	/* let's open the file*/
	matf = matOpen(file_name, "r" );
	if(!matf) {
		return ERROR_IO;
	}
	/* let's read the variable varname */
	Mx = matGetVariable(matf, varname);
	if (!Mx) {
		matClose(matf);
		return ERROR_IO_MAT;
	}
	/* let's close the file */
	res = matClose(matf);
	assert(!res);
	/* check: the variable must be a 2D matrix!! */
	if(mxGetNumberOfDimensions(Mx) > 2) {
		matClose(matf);
		return ERROR_IO_MAT;
	}
	/* let's create the Matrix */
	M->n_rows = (unsigned int) mxGetM(Mx);
	M->n_cols = (unsigned int) mxGetN(Mx);
	res = create_matrix(M, M->n_rows, M->n_cols);
	if (res) {
		mxDestroyArray(Mx);
		return res;
	}
	/* copy the data from the mxArray. Remember that matlab save in column-order!! */
	data = mxGetPr(Mx);
	for (j=0; j < M->n_cols; ++j) {
		for (i=0; i < M->n_rows; ++i) {
			matrix_set(M, i, j, (float) *data);
			++data;
		}
	}
	
	/* destroy mxArray*/
	mxDestroyArray(Mx);
	
	return 0;
}


int write_mat_matrix(const Matrix* M, const char* file_name, const char* varname)
{
	MATFile* matf = NULL;
	mxArray* Mx = NULL;
	unsigned int i, j;
	double* data;
	int res;
	
	assert(file_name);
	assert(M);
	assert(varname);
	
	/* create mxArray */
	Mx = mxCreateDoubleMatrix(M->n_rows, M->n_cols, mxREAL);
	if (!Mx) {
		exit(ERROR_MEM); /* TODO */
		return ERROR_MEM;
	}
	/* copy data in column order! */
	data = mxGetPr(Mx);
	for (j=0; j < M->n_cols; ++j) {
		for (i=0; i < M->n_rows; ++i) {
			*data = (double) matrix_get(M, i, j);
			++data;
		}
	}
	/* let's open the file */
	matf = matOpen(file_name, "wz" );
	if(!matf) {
		return ERROR_IO;
	}
	/* put the variable in the .mat */
	res = matPutVariable(matf, varname, Mx);
	if (res) {
		return ERROR_IO_MAT;
	}
	/* close the file and delete mxArray */
	res = matClose(matf);
	assert(!res);
	mxDestroyArray(Mx);
	
	return 0;
}


#endif
