/*
 * extract_picodes.h
 *
 *  Created on: Feb 14, 2012
 *      Author: alessandro
 */

#ifndef EXTRACT_PICODES_H_
#define EXTRACT_PICODES_H_

#include "extract_low_level_features.h"


/* [128 x D] Phi_picodes128_bf.dat
   [128 x D] Tau_picodes128_bf.dat
   [1024 x D] Phi_picodes1024_bf.dat
   [1024 x D] Tau_picodes1024_bf.dat
   [2048 x D] Phi_picodes2048_bf.dat
   [2048 x D] Tau_picodes2048_bf.dat
*/
#define PHI_PICODES128_BF "Phi_picodes128_bf.dat"
#define TAU_PICODES128_BF "Tau_picodes128_bf.dat"
#define PHI_PICODES1024_BF "Phi_picodes1024_bf.dat"
#define TAU_PICODES1024_BF "Tau_picodes1024_bf.dat"
#define PHI_PICODES2048_BF "Phi_picodes2048_bf.dat"
#define TAU_PICODES2048_BF "Tau_picodes2048_bf.dat"


struct struct_picodes {
	ImageData* img_data;
	Matrix* desc128;
	Matrix* desc1024;
	Matrix* desc2048;
};
typedef struct struct_picodes Picodes;

/* static functions */
int Picodes_init(const Program_Options* _options);
void Picodes_end();

/* methods */
void Picodes_constructor(Picodes* obj, ImageData* _img_data);
void Picodes_deconstructor(Picodes* obj);
int Picodes_get_descriptor128(Picodes* obj, Matrix** f_out);
int Picodes_get_descriptor1024(Picodes* obj, Matrix** f_out);
int Picodes_get_descriptor2048(Picodes* obj, Matrix** f_out);

#endif /* EXTRACT_PICODES_H_ */
