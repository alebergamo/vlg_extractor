/***************************************************************************
 *            homogeneous_kernel_map.h
 *
 *  Wed Jul 21 15:22:44 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 *
 *
 * [1] "Efficient additive kernels via explicit feature maps" by Andrea Vedaldi and Andrew Zisserman
 ****************************************************************************/

#ifndef HOMOGENEOUS_KERNEL_MAP_H
#define HOMOGENEOUS_KERNEL_MAP_H

#include "matrix.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Type of the kernels available
 */
typedef enum {
	None = 0, /* it is only to allow a null value for a HomogeneousKernelType variable */
	IntersectionKernel = 1, 
	ChiSquareKernel = 2
} HomogeneousKernelType;

/*
PhiX empty matrix
*/
void calculate_finite_feature_map(const Matrix* X, 
                                  unsigned int order,
                                  float L,
                                  HomogeneousKernelType kernel_type,
                                  float gamma,
                                  Matrix* PhiX
								  );



#ifdef __cplusplus
}
#endif

#endif /* HOMOGENEOUS_KERNEL_MAP_H */


