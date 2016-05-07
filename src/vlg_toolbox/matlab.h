/*
 *  matlab.h
 *  Xcode_vlg_toolbox
 *
 *  Created by Alessandro Bergamo on 2/11/12.
 *  Copyright 2012 Dartmouth College. All rights reserved.
 *
 */

#ifndef MATLAB_H
#define MATLAB_H

#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAT_SUPPORT

/* 
 ERROR:  ERROR_IO, ERROR_IO_MAT, [ERROR_MEM]
 */
int read_mat_matrix(Matrix* M, const char* file_name, const char* varname);

/*
 ERROR:  ERROR_IO, ERROR_IO_MAT, [ERROR_MEM] 
 */
int write_mat_matrix(const Matrix* M, const char* file_name, const char* varname);

#endif

#ifdef __cplusplus
}
#endif

#endif /* MATLAB_H */
