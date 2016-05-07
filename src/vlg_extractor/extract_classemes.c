/***************************************************************************
 *            extract_classemes.c
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "extract_classemes.h"
#include "error.h"

static const Program_Options* options = NULL;
static Matrix* Phi = NULL;
static Matrix* Tau = NULL;


int Classemes_init(const Program_Options* _options)
{
	char buffer[MAX_CHARACTERS_LINE];
	int res;

	assert(_options);

	options = _options;

	/* load Phi */
	Phi = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"PHI_CLASSEMES_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Phi, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}
	/* load Tau */
	Tau = (Matrix*) malloc(sizeof(Matrix));
	sprintf(buffer, "%s%c"TAU_CLASSEMES_BF, options->parameters_dir, SYSTEM_PATH_SEPARATOR);
	res = read_float_matrix(Tau, buffer);
	if(res) {
		return ERROR_PARAMETERS_LOADING;
	}

	return 0;
}



void Classemes_end()
{
	delete_matrix(Tau);
	free(Tau);
	delete_matrix(Phi);
	free(Phi);
}



void Classemes_constructor(Classemes* obj, ImageData* _img_data)
{
	assert(obj);
	assert(_img_data);

	obj->img_data = _img_data;
	obj->desc = NULL;
	obj->desc_bin = NULL;
}



void Classemes_deconstructor(Classemes* obj)
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
}



int Classemes_get_descriptor(Classemes* obj, Matrix** f_out)
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

	res = ImageData_get_low_level_feature_concatenation_IK(obj->img_data, &PsiX);;
	if (res) {
		return res;
	}
	/* compute classemes */
	/* the following line is temp_M = 1.0*Phi*PsiX + 1.0*Tau */
	temp_M = matrix_clone(Tau);
	matrix_times_matrix(temp_M, Phi, false, PsiX, false, 1.0, 1.0);

	obj->desc = temp_M;
	*f_out = obj->desc;

	return 0;
}



int Classemes_get_descriptorBIN(Classemes* obj, Matrix** f_out)
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

	res = Classemes_get_descriptor(obj, &temp_M);
	if (res) {
		return res;
	}
	obj->desc_bin = matrix_clone(temp_M);
	matrix_op_binarize(obj->desc_bin, 0.0);

	*f_out = obj->desc_bin;

	return 0;
}

