/*
 * bitmap.h
 *
 *  Created on: May 31, 2012
 *      Author: Alessandro Bergamo
 */

#ifndef BITMAP_H_
#define BITMAP_H_


#ifdef __cplusplus
extern "C" {
#endif


/* public
 * Initialize the look-up table. This function must be run just once
 */
void table_byte_index_init();



/**
 * Public.
 * Given a set of bits 'b', consisting of 'n_bytes', it returns the number of bit set to one.
 * If bias>=0 we assume an extra-bit set to one.
*/
unsigned int bitmap_sum(unsigned char* b, unsigned int n_bytes, double bias);


/**
 * Public.
 * Given a set of bits 'b', consisting of 'n_bytes', it writes on the array pointed by 'v_index', the index of the bits that are set to ones
 * (the leftmost bit has index zero).
 * 'n_index' is the number of bits are set to one.
 * Note: v_index must be pre-allocated and it must contains at least 8*n_bytes entries.
 * If bias>=0 we assume that the data has an extra-dimension which value is set to one.
 */
void bitmap_find(unsigned char* b, unsigned int n_bytes, unsigned int *v_index, unsigned int *n_index, double bias);


/* return 0 for false,  >0 for true*/
unsigned char get_bit(unsigned char* v, unsigned int i);


/*set the bit to one */
void set_bit(unsigned char* v, unsigned int i);


unsigned int endian_swap_uint32(unsigned int v);


unsigned long int endian_swap_uint64(unsigned long int v);


unsigned int get_uint_mask_right(unsigned int value);



#ifdef __cplusplus
}
#endif

#endif /* BITMAP_H_ */
