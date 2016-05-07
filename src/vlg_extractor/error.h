/***************************************************************************
 *            error.h
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef ERROR_H
#define ERROR_H


#define ERROR_IO -1
#define ERROR_IO_STR "ERROR Input/Output filesystem"
#define ERROR_MEM -2
#define ERROR_MEM_STR "ERROR Memory"
 /*during the get/putVariable operations. Variable does not exist or the matrix has more than 2 dimensions*/
#define ERROR_IO_MAT -3
#define ERROR_IO_MAT_STR "ERROR Input/Output matfile"
#define ERROR_PARSING_ARGUMENTS -4
#define ERROR_PARSING_ARGUMENTS_STR "ERROR while parsing program arguments"
#define ERROR_NOT_MAT_SUPPORT -5
#define ERROR_NOT_MAT_SUPPORT_STR "ERROR It has been requested a .mat feature but the support for the matlab file has not been enabled at compile time"
#define ERROR_MISSING_ARGUMENTS -6
#define ERROR_MISSING_ARGUMENTS_STR "ERROR Some required parameters are missing"
#define ERROR_BIN_OUTPUT -7
#define ERROR_BIN_OUTPUT_STR "It has been requested the output BIN but the output is not a vector"
#define ERROR_NOT_YET_IMPLEMENTED -8
#define ERROR_NOT_YET_IMPLEMENTED_STR "Some features requested are not been yet implemented. Contact the author."
#define ERROR_GENERIC -9
#define ERROR_GENERIC_STR "Error."
#define ERROR_BOW_FORMAT -10
#define ERROR_BOW_FORMAT_STR "ERROR BOW-FILE FORMAT"
#define ERROR_SIFT_EXTRACTION -11
#define ERROR_SIFT_EXTRACTION_STR "ERROR Generic error during the SIFT extraction"
#define ERROR_GIST_EXTRACTION -12
#define ERROR_GIST_EXTRACTION_STR "ERROR Generic error during the GIST extraction"
#define ERROR_PHOG_EXTRACTION -13
#define ERROR_PHOG_EXTRACTION_STR "ERROR Generic error during the PHOG extraction"
#define ERROR_SSIM_EXTRACTION -14
#define ERROR_SSIM_EXTRACTION_STR "ERROR Generic error during the SSIM extraction"
#define ERROR_LOADING_IMAGE -15
#define ERROR_PARAMETERS_LOADING -16

#endif


