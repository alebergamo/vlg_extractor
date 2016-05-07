/***************************************************************************
 *           extract_low_level_features.c
 *
 *  Sat Oct 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "extract_low_level_features.h"
#include <stdio.h>
#include <limits.h>
#include "error.h"
#include <sift/sift2.h>
#include <sift/sift_center.h>
#include <gist/extract_gist.h>
#include <gist/makePrefilter.h>
#include <gist/makeGabor.h>
#include <phog/phog_fc.h>
#include <ssim/hist_ssim.h>
#include <vlg_toolbox/opencv.h>
#include <vlg_toolbox/homogeneous_kernel_map.h>


static const char* feature_types[] = {"gist", "phog", "phog2pi", "ssim", "sift"};
static const unsigned int feature_levels[] =  { 0,      3,      3,         2,         2};
static const unsigned int feature_bins[]  =   { 960,    20,     40,        300,       500};
static const unsigned int ID_to_TYPE[] = {0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4};
static const unsigned int ID_TO_LEVEL[] = {0, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 0, 1, 2};

/* homogeneous kernel approximation variable */
static const unsigned int homkern_order = 1;
static const float homkern_L[] = {0.8, 0.8, 0.7, 0.7, 0.6, 0.8, 0.7, 0.7, 0.6, 0.6, 0.6, 0.7, 0.7, 0.7, 0.7};
static const HomogeneousKernelType homkern_type = IntersectionKernel;
static const float homkern_gamma = 1.0;
static const bool homkern_normalize = true;

static const Program_Options* options = NULL;
/* GIST variables */
static IplImage* gist_prefilter = NULL;
static IplImage* gist_gabor[GIST_NFILTERS];
/* SIFT variable */
static float siftcenters[NSIFTCENTER*128]; /* P.S.: ~625KB */
/* SSIM variables */
static float ssim_centers[SSIM_CDIM*SSIM_NCENTERS]; /* P.S.: ~8.7 KB */
static int* ssim_logpolar = NULL;
/* PHOG variables */
static int phog_bin360 = 40;
static int phog_bin180 = 20;
static int phog_L = 3;
static int phog_highthres = 115; /* canny detector threshold */


int ImageData_init(const Program_Options* _options)
{
	char buffer[MAX_CHARACTERS_LINE];
	int res;

	assert(_options);

	options = _options;

	/* GIST */
	/* prefilter */
	sprintf(buffer, "%s%cgist_prefilt32.bin", options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = makePrefilter(buffer, &gist_prefilter);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* gabor filter */
	sprintf(buffer, "%s%cgist_gabor_T.bin", options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = makeGabor(buffer, gist_gabor);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}

	/* SIFT */
	sprintf(buffer, "%s%csift_centers.bin", options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = sift_center(buffer, siftcenters);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}

	/* SSIM */
	/* kmeans centers */
	sprintf(buffer, "%s%cssim_centers.bin", options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = centereader(buffer, ssim_centers);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* logpolar mapping */
	sprintf(buffer, "%s%cssim_logpolarmask.bin", options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = mapreader(&ssim_logpolar, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}

	return 0;
}




void ImageData_end()
{
	unsigned int i;
	/* free the memory of the shared data*/
	cvReleaseImage(&gist_prefilter);
	for (i = 0; i < GIST_NFILTERS; ++i) {
		cvReleaseImage(&gist_gabor[i]);
	}
	free(ssim_logpolar);
}



int ImageData_constructor(ImageData* obj, const char* _image_in_fileName, const char* _image_out_fileName)
{
	unsigned int i;

	assert(obj);
	assert(_image_in_fileName);
	assert(_image_out_fileName);

	obj->img_in_fileName = string_clone(_image_in_fileName);
	obj->img_out_fileName = string_clone(_image_out_fileName);
	obj->img = NULL;

	for (i=0; i < NUM_LOW_LEVEL_FEATURES; ++i) {
		obj->low_level_features[i] = NULL;
	}
	for (i=0; i < NUM_LOW_LEVEL_FEATURES_PYRAMID; ++i) {
		obj->low_level_features_pyramid[i] = NULL;
		obj->low_level_features_pyramid_IK[i] = NULL;
	}
	obj->low_level_features_concatenation = NULL;
	obj->low_level_features_concatenation_IK = NULL;

	return 0;
}



void ImageData_deconstructor(ImageData* obj)
{
	unsigned int i;
	assert(obj);

	if (obj->low_level_features_concatenation_IK) {
		delete_matrix(obj->low_level_features_concatenation_IK);
		free(obj->low_level_features_concatenation_IK);
	}
	if (obj->low_level_features_concatenation) {
		delete_matrix(obj->low_level_features_concatenation);
		free(obj->low_level_features_concatenation);
	}
	for (i=0; i < NUM_LOW_LEVEL_FEATURES_PYRAMID; ++i) {
		if (obj->low_level_features_pyramid[i]) {
			delete_matrix(obj->low_level_features_pyramid[i]);
			free(obj->low_level_features_pyramid[i]);
		}
		if (obj->low_level_features_pyramid_IK[i]) {
			delete_matrix(obj->low_level_features_pyramid_IK[i]);
			free(obj->low_level_features_pyramid_IK[i]);
		}
	}
	for (i=0; i < NUM_LOW_LEVEL_FEATURES; ++i) {
		if (obj->low_level_features[i]) {
			delete_matrix(obj->low_level_features[i]);
			free(obj->low_level_features[i]);
		}
	}
	cvReleaseImage(&(obj->img));
	free(obj->img_in_fileName);
	free(obj->img_out_fileName);
}



int ImageData_get_low_level_feature(ImageData* obj, unsigned int id, Matrix** f_out)
{
	CvMat* temp_cvMat = NULL;
	CvMat* temp2_cvMat = NULL;
	int res;

	assert(obj);
	assert(f_out);
	assert(id < NUM_LOW_LEVEL_FEATURES);
	assert(id >= 0);

	/* if the feature has been already computed, we return it */
	if (obj->low_level_features[id]) {
		*f_out = obj->low_level_features[id];
		return 0;
	}

	/* load the image */
	if (!obj->img) {
		obj->img = cvLoadImage(obj->img_in_fileName, CV_LOAD_IMAGE_COLOR);
		if(!obj->img) {
			return ERROR_LOADING_IMAGE;
		}
	}

	switch(id) {
	case 0 : {
		/* +++ GIST +++ */
		temp_cvMat = cvCreateMat(NWINDOWS*NWINDOWS*NFILTERS*CHANNEL, 1, CV_32FC1);
		res = extract_gist(obj->img, gist_prefilter, gist_gabor, temp_cvMat);
		if(res) {
			cvReleaseMat(&temp_cvMat);
			return ERROR_GIST_EXTRACTION;
		}
		obj->low_level_features[0] = cvMat_2_Matrix(temp_cvMat);
		cvReleaseMat(&temp_cvMat);
		break;
	}
	case 1 :
	case 2 : {
		/* +++ PHOG + PHOG2PI +++ */
		temp_cvMat = cvCreateMat(PHOG_LEN360, 1, CV_32FC1);
		temp2_cvMat = cvCreateMat(PHOG_LEN180, 1, CV_32FC1);
		res = phog_fc(phog_bin360, phog_bin180, phog_L, phog_highthres, obj->img, temp_cvMat, temp2_cvMat);
		if(res) {
			cvReleaseMat(&temp_cvMat);
			cvReleaseMat(&temp2_cvMat);
			return ERROR_PHOG_EXTRACTION;
		}
		obj->low_level_features[1] = cvMat_2_Matrix(temp2_cvMat); /* 180 */
		obj->low_level_features[2] = cvMat_2_Matrix(temp_cvMat); /* 360 */
		cvReleaseMat(&temp_cvMat);
		cvReleaseMat(&temp2_cvMat);
		break;
	}
	case 3 : {
		/* +++ SSIM +++ */
		temp_cvMat = cvCreateMat(6300, 1, CV_32SC1);
		res = hist_ssim(obj->img, ssim_centers, ssim_logpolar, temp_cvMat, NULL);
		if(res) {
			cvReleaseMat(&temp_cvMat);
			return ERROR_SSIM_EXTRACTION;
		}
		obj->low_level_features[3] = cvMat_2_Matrix(temp_cvMat);
		cvReleaseMat(&temp_cvMat);
		break;
	}
	case 4 : {
		/* +++ SIFT  +++ */
		obj->low_level_features[4] = (Matrix*) malloc(sizeof(Matrix));
		res = sift_extraction(obj->img, obj->low_level_features[4], siftcenters);
		if(res) {
			free(obj->low_level_features[4]);
			obj->low_level_features[4] = NULL;
			return ERROR_SIFT_EXTRACTION;
		}
		break;
	}
	default : {
		assert(0);
		break; /* This is just to avoid a compiler-warning */
	}
	}

	*f_out = obj->low_level_features[id];

	return 0;
}



int ImageData_get_low_level_feature_pyramid(ImageData* obj, unsigned int id, Matrix** f_out)
{
	unsigned int idx_type, idx_level;
	unsigned int startbin, endbin;
	Matrix* temp_M = NULL;
	Matrix* feat = NULL;
	int res;

	assert(obj);
	assert(id >= 0);
	assert(id < NUM_LOW_LEVEL_FEATURES_PYRAMID);
	assert(f_out);

	idx_type = ID_to_TYPE[id];
	idx_level = ID_TO_LEVEL[id];

	/* if the feature has been already computed, we return it */
	if (obj->low_level_features_pyramid[id]) {
		*f_out = obj->low_level_features_pyramid[id];
		return 0;
	}

	/* retrieve the feature number idx_type */
	res = ImageData_get_low_level_feature(obj, idx_type, &feat);
	if (res) {
		return res;
	}

	/* let's compute the levels index and copy the portion of the vector */
	startbin = feature_bins[idx_type]*(pow(4.0,(double)idx_level) - 1)/3;
	endbin = -1 + feature_bins[idx_type]*(pow(4.0,(double)(idx_level+1)) - 1)/3;

	temp_M = (Matrix*) malloc(sizeof(Matrix));
	assert(temp_M);
	create_matrix(temp_M, endbin-startbin+1, 1);

	vector_cpy(temp_M, feat, startbin, endbin);
	obj->low_level_features_pyramid[id] = temp_M;
	*f_out = obj->low_level_features_pyramid[id];

	return 0;
}



int ImageData_get_low_level_feature_pyramid_IK(ImageData* obj, unsigned int id, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	Matrix* feat = NULL;
	Matrix* PsiX = NULL;
	int res;

	assert(obj);
	assert(id >= 0);
	assert(id < NUM_LOW_LEVEL_FEATURES_PYRAMID);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->low_level_features_pyramid_IK[id]) {
		*f_out = obj->low_level_features_pyramid_IK[id];
		return 0;
	}

	res = ImageData_get_low_level_feature_pyramid(obj, id, &feat);
	if (res) {
		return res;
	}
	temp_M = matrix_clone(feat);
	if (homkern_normalize) {
		matrix_normalize_p_norm(temp_M, homkern_gamma);
	}
	PsiX = (Matrix*) malloc(sizeof(Matrix));
	calculate_finite_feature_map(temp_M, homkern_order, homkern_L[id], homkern_type, homkern_gamma, PsiX);
	obj->low_level_features_pyramid_IK[id] = PsiX;
	*f_out = obj->low_level_features_pyramid_IK[id];

	/* free */
	delete_matrix(temp_M);
	free(temp_M);

	return 0;
}



int ImageData_get_low_level_feature_concatenation(ImageData* obj, Matrix** f_out)
{
	Matrix* feat[15];
	Matrix* temp_M = NULL;
	unsigned int i;
	int res;
	char buffer[MAX_CHARACTERS_LINE];
	unsigned int id, startbin, offset;

	/* if the feature has been already computed, we return it */
	if (obj->low_level_features_concatenation) {
		*f_out = obj->low_level_features_concatenation;
		return 0;
	}

	/* if requested, load the file from the disk */
	if (options->load_concatenationLowLevelFeatures.type != IOTYPE_NDEF_TYPE) {
		temp_M = (Matrix*) malloc(sizeof(Matrix));
		sprintf(buffer, "%s%s", obj->img_out_fileName, options->load_concatenationLowLevelFeatures.suffix);
		res = read_input(buffer, temp_M, &options->load_concatenationLowLevelFeatures);
		assert(!res);
		//obj->low_level_features_concatenation = temp_M;
		//*f_out = obj->low_level_features_concatenation;
		/* let's "disassemble" the vector*/
		startbin = 0;
		for (id=0; id < 5; ++id) {
			offset = feature_bins[id]*(pow(4.0,(double)(feature_levels[id]+1)) - 1)/3;
			if (!obj->low_level_features[id]) {
				obj->low_level_features[id] = (Matrix*) malloc(sizeof(Matrix));
				assert(obj->low_level_features[id]);
				create_matrix(obj->low_level_features[id], offset, 1);
				vector_cpy(obj->low_level_features[id], temp_M, startbin, startbin+offset-1);
			}

			startbin += offset;
		}
		/* free */
		delete_matrix(temp_M);
		free(temp_M);
		temp_M = NULL;
	}

	for (i=0; i < 15; ++i) {
		res = ImageData_get_low_level_feature_pyramid(obj, i, &(feat[i]));
		if (res) {
			return res;
		}
	}

	obj->low_level_features_concatenation = concatenate_vectors(feat, 15);
	*f_out = obj->low_level_features_concatenation;

	return 0;
}



int ImageData_get_low_level_feature_concatenation_IK(ImageData* obj, Matrix** f_out)
{
	Matrix* feat[15];
	unsigned int i;
	int res;

	/* if the feature has been already computed, we return it */
	if (obj->low_level_features_concatenation_IK) {
		*f_out = obj->low_level_features_concatenation_IK;
		return 0;
	}

	for (i=0; i < 15; ++i) {
		res = ImageData_get_low_level_feature_pyramid_IK(obj, i, &(feat[i]));
		if (res) {
			return res;
		}
	}

	obj->low_level_features_concatenation_IK = concatenate_vectors(feat, 15);
	*f_out = obj->low_level_features_concatenation_IK;

	return 0;
}



