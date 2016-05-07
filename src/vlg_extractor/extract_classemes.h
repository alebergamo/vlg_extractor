/***************************************************************************
 *            extract_classemes.h
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef EXTRACT_CLASSEMES_H
#define EXTRACT_CLASSEMES_H

#include "extract_low_level_features.h"


/* [C x D] Phi_classemes_bf.dat
   [C x D] Tau_classemes_bf.dat
*/
#define PHI_CLASSEMES_BF "Phi_classemes_bf.dat"
#define TAU_CLASSEMES_BF "Tau_classemes_bf.dat"


struct struct_classemes {
	ImageData* img_data;
	Matrix* desc;
	Matrix* desc_bin;
};
typedef struct struct_classemes Classemes;

/* static functions */
int Classemes_init(const Program_Options* _options);
void Classemes_end();

/* methods */
void Classemes_constructor(Classemes* obj, ImageData* _img_data);
void Classemes_deconstructor(Classemes* obj);
int Classemes_get_descriptor(Classemes* obj, Matrix** f_out);
int Classemes_get_descriptorBIN(Classemes* obj, Matrix** f_out);


#endif /* EXTRACT_CLASSEMES_H */


