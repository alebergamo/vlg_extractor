/*
 * extract_picodes.c
 *
 *  Created on: Feb 14, 2012
 *      Author: alessandro
 */

#include "extract_picodes.h"
#include "error.h"

static const Program_Options* options = NULL;
static Matrix* Phi128 = NULL;
static Matrix* Tau128 = NULL;
static Matrix* Phi1024 = NULL;
static Matrix* Tau1024 = NULL;
static Matrix* Phi2048 = NULL;
static Matrix* Tau2048 = NULL;

int Picodes_init(const Program_Options* _options)
{
	char buffer[MAX_CHARACTERS_LINE];
	int res;

	assert(_options);

	options = _options;

	/* load 128 */
	Phi128 = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"PHI_PICODES128_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Phi128, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	Tau128 = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"TAU_PICODES128_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Tau128, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* load 1024 */
	Phi1024 = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"PHI_PICODES1024_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Phi1024, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	Tau1024 = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"TAU_PICODES1024_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Tau1024, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* load 2048 */
	Phi2048 = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"PHI_PICODES2048_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Phi2048, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	Tau2048 = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"TAU_PICODES2048_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Tau2048, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}

	return 0;
}


void Picodes_end()
{
	delete_matrix(Tau2048);
	free(Tau2048);
	delete_matrix(Phi2048);
	free(Phi2048);

	delete_matrix(Tau1024);
	free(Tau1024);
	delete_matrix(Phi1024);
	free(Phi1024);

	delete_matrix(Tau128);
	free(Tau128);
	delete_matrix(Phi128);
	free(Phi128);
}


void Picodes_constructor(Picodes* obj, ImageData* _img_data)
{
	assert(obj);
	assert(_img_data);

	obj->img_data = _img_data;
	obj->desc128 = NULL;
	obj->desc1024 = NULL;
	obj->desc2048 = NULL;
}


void Picodes_deconstructor(Picodes* obj)
{
	assert(obj);

	if (obj->desc128) {
		delete_matrix(obj->desc128);
		free(obj->desc128);
	}
	if (obj->desc1024) {
		delete_matrix(obj->desc1024);
		free(obj->desc1024);
	}
	if (obj->desc2048) {
		delete_matrix(obj->desc2048);
		free(obj->desc2048);
	}
}


int Picodes_get_descriptor128(Picodes* obj, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	Matrix* PsiX = NULL;
	int res;

	assert(obj);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->desc128) {
		*f_out = obj->desc128;
		return 0;
	}

	res = ImageData_get_low_level_feature_concatenation_IK(obj->img_data, &PsiX);;
	if (res) {
		return res;
	}
	/* compute picodes */
	temp_M = matrix_clone(Tau128);
	matrix_times_matrix(temp_M, Phi128, false, PsiX, false, 1.0, 1.0);
	matrix_op_binarize(temp_M, 0.0);

	obj->desc128 = temp_M;
	*f_out = obj->desc128;

	return 0;
}


int Picodes_get_descriptor1024(Picodes* obj, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	Matrix* PsiX = NULL;
	int res;

	assert(obj);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->desc1024) {
		*f_out = obj->desc1024;
		return 0;
	}

	res = ImageData_get_low_level_feature_concatenation_IK(obj->img_data, &PsiX);;
	if (res) {
		return res;
	}
	/* compute picodes */
	temp_M = matrix_clone(Tau1024);
	matrix_times_matrix(temp_M, Phi1024, false, PsiX, false, 1.0, 1.0);
	matrix_op_binarize(temp_M, 0.0);

	obj->desc1024 = temp_M;
	*f_out = obj->desc1024;

	return 0;
}


int Picodes_get_descriptor2048(Picodes* obj, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	Matrix* PsiX = NULL;
	int res;

	assert(obj);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->desc2048) {
		*f_out = obj->desc2048;
		return 0;
	}

	res = ImageData_get_low_level_feature_concatenation_IK(obj->img_data, &PsiX);;
	if (res) {
		return res;
	}
	/* compute picodes */
	temp_M = matrix_clone(Tau2048);
	matrix_times_matrix(temp_M, Phi2048, false, PsiX, false, 1.0, 1.0);
	matrix_op_binarize(temp_M, 0.0);

	obj->desc2048 = temp_M;
	*f_out = obj->desc2048;

	return 0;
}

