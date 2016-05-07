/*
 * sift_center.c
 *
 *  Created on: Jan 9, 2011
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include "sift_center.h"


int sift_center( char* path, float* centers)
{
	FILE* centerFile=fopen(path,"rb");
	if (!centerFile) {
		return ERROR_SIFT_CENTER_READ_PROBLEM;
	}
	fread(centers,sizeof(float),NSIFTCENTER*128,centerFile);
	fclose(centerFile);

	return 0;
}

