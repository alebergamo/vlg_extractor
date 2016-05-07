/*
 * siftpp_wrapper.h
 *
 *  Created on: Dec 15, 2010
 *      Author: Alessandro Bergamo
 */

#ifndef SIFTPP_WRAPPER_H_
#define SIFTPP_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is just a test function. It prints a message.
 * */
void siftpp_wrapper_test();

/*
 * Compute SIFT descriptors using Vedaldi's code
 *
 * INPUT:
 *   data: input image. It is expected a gray image, 8 bit channel (unsigned char), row by row
 *   width: with of the image
 *   height
 *
 * OUTPUT:
 *	n_keypoints: number of keypoints that have been located
 *  descriptors: it is a vector with all the <n_keypoints> descriptors concatenated, plus the positions.
 *               (each descriptors is organized as follow:   [<x> <y> <128 entries>])
 *               P.S.: remember to release the memory of <descriptors>!!!
 *
 * RETURN:
 *   0 success
 *   -1 error
 * */
int compute_sift_descriptors(unsigned char* data, int width, int height, int* n_descriptors, double** descriptors);


#ifdef __cplusplus
}
#endif



#endif /* SIFTPP_WRAPPER_H_ */
