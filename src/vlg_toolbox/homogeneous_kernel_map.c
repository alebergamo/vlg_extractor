/***************************************************************************
 *            homogeneous_kernel_map.c
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "homogeneous_kernel_map.h"
#include <math.h>
#include <assert.h>

void calculate_finite_feature_map(const Matrix* X, 
                                  unsigned int order,
                                  float L,
                                  HomogeneousKernelType kernel_type,
                                  float gamma,
                                  Matrix* PhiX
								  )
{
unsigned int nDim;
unsigned int nSamples;
unsigned int d, i, offset, n;  
float x;
float sqrtL = sqrt(L);
float sqrtXL;
float lambda;
float logX;
float sqrtKappa0;
float sqrt2Kappa;

assert(X);
assert(PhiX);

nDim = X->n_rows;
nSamples = X->n_cols;
  
create_matrix(PhiX, nDim*(2*order+1), nSamples);

for (i=0; i < nSamples; ++i) {
  offset = 0;
  for (d=0; d < nDim; ++d) {
	x = matrix_get(X, d, i);
	
	sqrtXL = sqrtL * (float)pow((double)x, (double)(gamma/2.0));
	if (fabs(x) < (float)pow(10.0, -10.0)) {
	  x = (float)pow(10.0, -10.0); 
	  sqrtXL = 0.0;
	  /* Remember that an homogeneous kernel is defined as: k:R_0^+ x R_0^+ -> R_0^+
	    The log on below will not work if x==0. However we can say that lim_{x->0} sqrt(XL) = 0, so \hat{\Psi}(x) = 0
	    So, using this trick the following log works, and \hat{\Psi}(x) = 0
	  */
	}
	logX = log(x);
	switch (kernel_type) {
	  case IntersectionKernel : {
		sqrtKappa0 = sqrt(2.0 / PI);
		/* lambda=0 */
		matrix_set(PhiX, offset, i, sqrtKappa0*sqrtXL);
		++offset;
		/* lambda={L, ..., n*L}*/
		for (n=1; n <= order; ++n) {
		  lambda = L*n;
		  sqrt2Kappa = sqrt(2.0* ((2.0/PI) * 1.0/(1+4*pow(lambda,2))));
		  matrix_set(PhiX, offset, i, sqrt2Kappa*sqrtXL*cos(lambda*logX));
		  ++offset;
		  matrix_set(PhiX, offset, i, sqrt2Kappa*sqrtXL*sin(lambda*logX));
		  ++offset;
		}
		break;
	  }
	  case ChiSquareKernel : {
		sqrtKappa0 = 1.0;
		/* lambda=0 */
		matrix_set(PhiX, offset, i, sqrtKappa0*sqrtXL);
		++offset;
		/* lambda={L, ..., n*L}*/
		for (n=1; n <= order; ++n) {
			lambda = L*n;
			sqrt2Kappa = sqrt(2.0*(2.0 / (exp(-PI*lambda) + exp(PI*lambda))));
			matrix_set(PhiX, offset, i, sqrt2Kappa*sqrtXL*cos(lambda*logX));
			++offset;
			matrix_set(PhiX, offset, i, sqrt2Kappa*sqrtXL*sin(lambda*logX));
			++offset;
		}
		break;
	  }
	  default : {
		assert(false);
	  }
	}
  }
}
}


