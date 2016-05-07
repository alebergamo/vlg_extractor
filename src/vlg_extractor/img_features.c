/***************************************************************************
 *            img_features.c
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "img_features.h"
#include <vlg_toolbox/utils.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

int create_img_features(ImgFeatures* imgf, unsigned int capacity)
{
	assert(imgf);
	assert(capacity > 0);

	imgf->name = (char**) malloc(capacity*sizeof(char*));
	imgf->level =  (unsigned int*) malloc(capacity*sizeof(unsigned int));
	imgf->data = (Matrix**) malloc(capacity*sizeof(Matrix*));
	imgf->capacity = capacity;
	imgf->n_features = 0;
	
	if (!(imgf->name && imgf->level && imgf->data)) {
		fprintf(stderr, ERROR_MEM_STR" %s %d\n", __FILE__, __LINE__);
		exit(ERROR_MEM);
		return ERROR_MEM;
	} else {
		return 0;
	}
}

void delete_img_features(ImgFeatures* imgf)
{
	unsigned int i;
  
	assert(imgf);
	
	for (i=0; i < imgf->n_features; ++i) {
		free(imgf->name[i]);
		delete_matrix(imgf->data[i]);
		free(imgf->data[i]);
	}
	free(imgf->name);
	free(imgf->level);
	free(imgf->data);
}

void add_feature(ImgFeatures* imgf, char* name, unsigned int level, Matrix* data)
{
	unsigned int i;
  
	assert(imgf);
	assert(name);
	assert(data);
	assert(imgf->n_features < imgf->capacity);
	
	i = imgf->n_features;
	imgf->name[i] = name;
	imgf->level[i] = level;
	imgf->data[i] = data;
	imgf->n_features = i+1;
}

const char* get_feature_name(ImgFeatures* imgf, unsigned int k)
{
	const char* res;
  
	assert(imgf);
	assert(k < imgf->n_features);
	
	res = imgf->name[k];
	
	return res;
}

unsigned int get_feature_level(ImgFeatures* imgf, unsigned int k)
{
	unsigned int res;
  
	assert(imgf);
	assert(k < imgf->n_features);
	
	res = imgf->level[k];
	
	return res;
}

Matrix* get_feature_vector(ImgFeatures* imgf, unsigned int k)
{
	Matrix* res = NULL;
  
	assert(imgf);
	assert(k < imgf->n_features);
	
	res = imgf->data[k];
	
	return res;
}

void print_img_feature(ImgFeatures* imgf)
{
	unsigned int i;
	
	assert(imgf);
	
	printf("n_features %d\n", imgf->n_features);
	for (i=0; i < imgf->n_features; ++i) {
		printf("%s %d [%d %d]\n", imgf->name[i], imgf->level[i], (imgf->data[i])->n_rows, (imgf->data[i])->n_cols);
	}
}


