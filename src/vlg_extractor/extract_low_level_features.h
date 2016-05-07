/***************************************************************************
 *           extract_low_level_features.h
 *
 *  Sat Oct 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef EXTRACT_LOW_LEVEL_FEATURES_H
#define EXTRACT_LOW_LEVEL_FEATURES_H

#include <vlg_toolbox/matrix.h>
#include <vlg_toolbox/utils.h>
#include <gist/extract_gist.h>
#include <gist/makePrefilter.h>
#include <gist/makeGabor.h>
#include <phog/phog_fc.h>
#include <ssim/hist_ssim.h>
#include <sift/sift2.h>
#include <sift/sift_center.h>
#include "vlg_extractor.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

/* structure of the low-level
 * name feat.| Lev. | size
----------------------------------
gist 0 [C, 960]
phog 0 [C, 20]
phog 1 [C, 80]
phog 2 [C, 320]
phog 3 [C, 1280]
phog2pi 0 [C, 40]
phog2pi 1 [C, 160]
phog2pi 2 [C, 640]
phog2pi 3 [C, 2560]
ssim_pw 0 [C, 300]
ssim_pw 1 [C, 1200]
ssim_pw 2 [C, 4800]
bow500 0 [C, 500]
bow500 1 [C, 2000]
bow500 2 [C, 8000]
 * */
#define NUM_LOW_LEVEL_FEATURES 5
#define NUM_LOW_LEVEL_FEATURES_PYRAMID 15


/* ******** fields   */
struct struct_ImageData {
	char* img_in_fileName;
	char* img_out_fileName;
	IplImage* img;
	Matrix* low_level_features[NUM_LOW_LEVEL_FEATURES];
	Matrix* low_level_features_pyramid[NUM_LOW_LEVEL_FEATURES_PYRAMID];
	Matrix* low_level_features_pyramid_IK[NUM_LOW_LEVEL_FEATURES_PYRAMID];
	Matrix* low_level_features_concatenation;
	Matrix* low_level_features_concatenation_IK;
};
typedef struct struct_ImageData ImageData;


/* ****** static functions */
/* != 0 if error */
int ImageData_init(const Program_Options* options);
void ImageData_end();

/*  ***** static data */
/*   all the private data are at the beginning of the .c */


/* ****** methods */
/* image_fileName will be copied */
int ImageData_constructor(ImageData* obj, const char* _image_in_fileName, const char* _image_out_fileName);
void ImageData_deconstructor(ImageData* obj);

int ImageData_get_low_level_feature(ImageData* obj, unsigned int id, Matrix** f_out);
int ImageData_get_low_level_feature_pyramid(ImageData* obj, unsigned int id, Matrix** f_out);
int ImageData_get_low_level_feature_pyramid_IK(ImageData* obj, unsigned int id, Matrix** f_out);
int ImageData_get_low_level_feature_concatenation(ImageData* obj, Matrix** f_out);
int ImageData_get_low_level_feature_concatenation_IK(ImageData* obj, Matrix** f_out);


#endif
