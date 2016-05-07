/*
 * extract_metaclass.c
 *
 *  Created on: Feb 17, 2012
 *      Author: Alessandro Bergamo
 */

#include "extract_metaclass.h"
#include "error.h"

static const Program_Options* options = NULL;
static Matrix* Phi = NULL;
static Matrix* Tau = NULL;
static Matrix* Sig_T = NULL;
static Matrix* Sig_B = NULL;


int Metaclass_init(const Program_Options* _options)
{
	char buffer[MAX_CHARACTERS_LINE];
	int res;

	assert(_options);

	options = _options;

	/* load Phi */
	Phi = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"PHI_MC_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Phi, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* load Tau */
	Tau = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"TAU_MC_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Tau, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* load Sig_T */
	Sig_T = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"SIG_T_MC_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Sig_T, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* load Sig_B */
	Sig_B = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"SIG_B_MC_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Sig_B, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}

	return 0;
}


void Metaclass_end()
{
	delete_matrix(Tau);
	free(Tau);
	delete_matrix(Phi);
	free(Phi);
	delete_matrix(Sig_T);
	free(Sig_T);
	delete_matrix(Sig_B);
	free(Sig_B);
}


void Metaclass_constructor(Metaclass* obj, ImageData* _img_data)
{
	assert(obj);
	assert(_img_data);

	obj->img_data = _img_data;
	obj->desc = NULL;
	obj->desc_bin = NULL;
	obj->desc_platt = NULL;
}


void Metaclass_deconstructor(Metaclass* obj)
{
	assert(obj);

	if (obj->desc) {
		delete_matrix(obj->desc);
		free(obj->desc);
	}
	if (obj->desc_bin) {
		delete_matrix(obj->desc_bin);
		free(obj->desc_bin);
	}
	if (obj->desc_platt) {
		delete_matrix(obj->desc_platt);
		free(obj->desc_platt);
	}
}



int Metaclass_get_descriptor(Metaclass* obj, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	Matrix* PsiX = NULL;
	int res;

	assert(obj);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->desc) {
		*f_out = obj->desc;
		return 0;
	}

	res = ImageData_get_low_level_feature_concatenation_IK(obj->img_data, &PsiX);
	if (res) {
		return res;
	}
	/* compute descriptor */
	/* the following line is temp_M = 1.0*Phi*PsiX + 1.0*Tau */
	temp_M = matrix_clone(Tau);
	matrix_times_matrix(temp_M, Phi, false, PsiX, false, 1.0, 1.0);

	obj->desc = temp_M;
	*f_out = obj->desc;

	return 0;
}


int Metaclass_get_descriptorPLATT(Metaclass* obj, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	int res;

	assert(obj);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->desc_platt) {
		*f_out = obj->desc_platt;
		return 0;
	}

	res = Metaclass_get_descriptor(obj, &temp_M);
	if (res) {
		return res;
	}
	obj->desc_platt = matrix_clone(temp_M);
	matrix_op_platt(obj->desc_platt, Sig_T, Sig_B);

	*f_out = obj->desc_platt;

	return 0;
}


int Metaclass_get_descriptorBIN(Metaclass* obj, Matrix** f_out)
{
	Matrix* temp_M = NULL;
	int res;

	assert(obj);
	assert(f_out);

	/* if the feature has been already computed, we return it */
	if (obj->desc_bin) {
		*f_out = obj->desc_bin;
		return 0;
	}

	res = Metaclass_get_descriptor(obj, &temp_M);
	if (res) {
		return res;
	}
	obj->desc_bin = matrix_clone(temp_M);
	matrix_op_binarize(obj->desc_bin, 0.0);

	*f_out = obj->desc_bin;

	return 0;
}
