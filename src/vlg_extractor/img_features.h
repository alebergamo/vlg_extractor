/***************************************************************************
 *            img_features.h
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef IMG_FEATURES_H
#define IMG_FEATURES_H

#include <vlg_toolbox/matrix.h>

struct struct_img_features {
	char** name; /* private*/
	unsigned int* level; /* private */
	Matrix** data; /* private*/
	
	unsigned int capacity; /* private */
	unsigned int n_features; /* public read-only */
};
typedef struct struct_img_features ImgFeatures;

/* [ERROR MEM] */
int create_img_features(ImgFeatures* imgf, unsigned int capacity);

/* this procedure delete the internal structure of imgf. N.B. do not execute free(imgf) */
void delete_img_features(ImgFeatures* imgf);

/* N.B. name must the a valid pointer (so a stack-string is not good because it will be deleted!!!)*/
void add_feature(ImgFeatures* imgf, char* name, unsigned int level, Matrix* data);

const char* get_feature_name(ImgFeatures* imgf, unsigned int k);

unsigned int get_feature_level(ImgFeatures* imgf, unsigned int k);

/*to note: it returns Matrix* so the data can be modified */
Matrix* get_feature_vector(ImgFeatures* imgf, unsigned int k);

void print_img_feature(ImgFeatures* imgf);

#endif
