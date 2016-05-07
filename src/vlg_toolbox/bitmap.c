/*
 * bitmap.c
 *
 *  Created on: May 31, 2012
 *      Author: Alessandro Bergamo
 *
 */

#include "bitmap.h"
#include <assert.h>

/* *** structure and functions to support bitmap data *** */
/* This is a table [256 x 9]
 * Each row is associated with a byte (a number 0..255).
 * T[i,0] contains the number of indexes
 * T[i,1]..T[i,T[i,0]] are the real indexes, from 0 (most left bit) to 7 (most right)
 * E.g.: the first rows are:
 * --------------------
 * 0:   0 ? ? ? ? ? ? ? ?
 * 1:   1 7 ? ? ? ? ? ? ?    1==0b00000001
 * 2:   1 6 ? ? ? ? ? ? ?    2==0b00000010
 * 3:   2 6 7 ? ? ? ? ? ?    3==0b00000011
 * etc...
 * 255: 8 1 2 3 4 5 6 7 8    255==0b11111111
 * -------------------
 * */
// private
unsigned int TABLE_BYTE_INDEX[256*9];


void table_byte_index_init()
{
	unsigned int i;
	unsigned int offset;

	for (i=0; i < 256; ++i) {
		offset = 1;
		if (i & 0x80) { TABLE_BYTE_INDEX[i*9+offset] = 0; ++offset; }
		if (i & 0x40) { TABLE_BYTE_INDEX[i*9+offset] = 1; ++offset; }
		if (i & 0x20) { TABLE_BYTE_INDEX[i*9+offset] = 2; ++offset; }
		if (i & 0x10) { TABLE_BYTE_INDEX[i*9+offset] = 3; ++offset; }
		if (i & 0x08) { TABLE_BYTE_INDEX[i*9+offset] = 4; ++offset; }
		if (i & 0x04) { TABLE_BYTE_INDEX[i*9+offset] = 5; ++offset; }
		if (i & 0x02) { TABLE_BYTE_INDEX[i*9+offset] = 6; ++offset; }
		if (i & 0x01) { TABLE_BYTE_INDEX[i*9+offset] = 7; ++offset; }
		TABLE_BYTE_INDEX[i*9] = offset-1;
	}
}


/*
 * Private.
 * Given 8 bits, it returns the number of bits are set to one (exploiting the hash-table)
 * */
unsigned int hashtable_byte_index_sum(unsigned char b)
{
	return TABLE_BYTE_INDEX[b*9];
}


/*
 * Private.
 * Given 8 bits (uchar 'c'), it writes on the array pointed by 'v_index', the index of the bits that are set to ones; if offset ~= 0, we add offset to the index.
 * The index most left bit is 0 and the most right 7. 'n_index' is the number of bits are set to one.
 * Note: v_index must be pre-allocated and it must contains at least 8 entries.
 * */
void hashtable_byte_index_find(unsigned char b, unsigned int *v_index, unsigned int *n_index, unsigned int offset)
{
	// ** 002
	/*(*n_index) = 0;
	if (b & 0x80) { (*v_index) = 0 + offset; ++v_index; ++(*n_index); }
	if (b & 0x40) { (*v_index) = 1 + offset; ++v_index; ++(*n_index); }
	if (b & 0x20) { (*v_index) = 2 + offset; ++v_index; ++(*n_index); }
	if (b & 0x10) { (*v_index) = 3 + offset; ++v_index; ++(*n_index); }
	if (b & 0x08) { (*v_index) = 4 + offset; ++v_index; ++(*n_index); }
	if (b & 0x04) { (*v_index) = 5 + offset; ++v_index; ++(*n_index); }
	if (b & 0x02) { (*v_index) = 6 + offset; ++v_index; ++(*n_index); }
	if (b & 0x01) { (*v_index) = 7 + offset; ++v_index; ++(*n_index); }*/

	// ** 001
	unsigned int i;
	*n_index = TABLE_BYTE_INDEX[b*9];
	for (i=0; i < *n_index; ++i) {
		v_index[i] = TABLE_BYTE_INDEX[b*9+1+i]+offset;
	}
	//memcpy(v_index, &TABLE_BYTE_INDEX[b*9+1], *n_index); // special case if offset==0
}


unsigned int bitmap_sum(unsigned char* b, unsigned int n_bytes, double bias)
{
	unsigned int i;
	unsigned int s = 0;
	for (i=0; i < n_bytes; ++i) {
		s += hashtable_byte_index_sum(b[i]);
	}
	s += (bias >= 0) ? 1 : 0;
	return s;
}


void bitmap_find(unsigned char* b, unsigned int n_bytes, unsigned int *v_index, unsigned int *n_index, double bias)
{
	unsigned int i;
	unsigned int offset;
	unsigned int temp_uint;

	offset = 0;
	for (i=0; i < n_bytes; ++i) {
		hashtable_byte_index_find(b[i], v_index+offset, &temp_uint, i*8);
		offset += temp_uint;
	}
	if (bias >= 0) {
		v_index[offset] = n_bytes*8;
		++offset;
	}
	*n_index = offset;
}



unsigned char get_bit(unsigned char* v, unsigned int i)
{
	v = v + (i/8);
	switch (i%8) {
	case 0: { return (*v & 0x80); }
	case 1: { return (*v & 0x40); }
	case 2: { return (*v & 0x20); }
	case 3: { return (*v & 0x10); }
	case 4: { return (*v & 0x08); }
	case 5: { return (*v & 0x04); }
	case 6: { return (*v & 0x02); }
	case 7: { return (*v & 0x01); }
	default: {
		assert(0);
		return 0xFF;
	}
	}
}


void set_bit(unsigned char* v, unsigned int i)
{
	v = v + (i/8);
	switch (i%8) {
	case 0: { *v |= 0x80; break; }
	case 1: { *v |= 0x40; break; }
	case 2: { *v |= 0x20; break; }
	case 3: { *v |= 0x10; break; }
	case 4: { *v |= 0x08; break; }
	case 5: { *v |= 0x04; break; }
	case 6: { *v |= 0x02; break; }
	case 7: { *v |= 0x01; break; }
	default: {
		assert(0);
		break;
	}
	}
}


/* TODO to implement for different compilers. E.g. Visual C++ offers __byteswap_ulong  */
unsigned int endian_swap_uint32(unsigned int v) {
	unsigned int res = 0;
#if GCC_VERSION >= 40300
	res = __builtin_bswap32(v);
#else
   // generic implementation
   res = (v >> 24) |
		 ((v<<8) && 0x00FF0000) |
		 ((v>>8) && 0x0000FF00) |
		 (v << 24);
#endif
	return res;
}

/* TODO to implement for different compilers. E.g. Visual C++ offers __byteswap_uint64  */
unsigned long int endian_swap_uint64(unsigned long int v) {
	unsigned long int res = 0;
#if GCC_VERSION >= 40300
	res = __builtin_bswap64(v);
#else
    res = (v >> 56) |
          ((v<<40) && 0x00FF000000000000) |
          ((v<<24) && 0x0000FF0000000000) |
          ((v<<8) && 0x000000FF00000000) |
          ((v>>8) && 0x00000000FF000000) |
          ((v>>24) && 0x0000000000FF0000) |
          ((v>>40) && 0x000000000000FF00) |
          (v << 56);
#endif
	return res;
}

/* public
 * Return an uint such that the most 'value' right bit are set to one
 * */
unsigned int get_uint_mask_right(unsigned int value) {
	unsigned int res = 0xFFFFFFFF;
	if (value==0) {
		res = 0;
	} else {
		res >>= 8*sizeof(unsigned int)-value;
	}
	return res;
}

