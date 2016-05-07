/*
 * extract_metaclass.h
 *
 *  Created on: Feb 17, 2012
 *      Author: Alessandro Bergamo
 */

#ifndef EXTRACT_METACLASS_H_
#define EXTRACT_METACLASS_H_


#include "extract_low_level_features.h"

/* [C x D] Phi_mc_bf.dat
   [C x D] Tau_mc_bf.dat
   [C x 1] Sig_T_mc_bf.dat
   [C x 1] Sig_B_mc_bf.dat
*/
#define PHI_MC_BF "Phi_mc_bf.dat"
#define TAU_MC_BF "Tau_mc_bf.dat"
#define SIG_T_MC_BF "Sig_T_mc_bf.dat"
#define SIG_B_MC_BF "Sig_B_mc_bf.dat"


struct struct_metaclass {
	ImageData* img_data;
	Matrix* desc;
	Matrix* desc_platt;
	Matrix* desc_bin;
};
typedef struct struct_metaclass Metaclass;

/* static functions */
int Metaclass_init(const Program_Options* _options);
void Metaclass_end();

/* methods */
void Metaclass_constructor(Metaclass* obj, ImageData* _img_data);
void Metaclass_deconstructor(Metaclass* obj);
int Metaclass_get_descriptor(Metaclass* obj, Matrix** f_out);
int Metaclass_get_descriptorPLATT(Metaclass* obj, Matrix** f_out);
int Metaclass_get_descriptorBIN(Metaclass* obj, Matrix** f_out);



#endif /* EXTRACT_METACLASS_H_ */
