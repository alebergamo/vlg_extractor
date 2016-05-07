/*
 *  quantize.cpp
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include "quantize.h"
#include <stdlib.h>

int quantize(vector<float> descriptors, vector<int> pointsX, vector<int> pointsY,int* & hist,float* centers,int row, int col)
{
	int i,j,k;
	//hist=(int*) malloc(sizeof(int)*NCHANNEL);
	for(i=0;i<NCHANNEL;i++)
	{
		hist[i]=0;
	}
	float best_dist=FLT_MAX;
	float dist=0;
	int length=(descriptors.size()-1)/CDIM;
	int *nearestLable=(int *) malloc(sizeof(int)*length);
	for(i=0;i<length;i++)
	{
		int index=0;
		for(j=0;j<NCENTERS;j++)
		{
			for(k=0;k<CDIM;k++)
			{
				dist=dist+(descriptors[30*i+k]-centers[30*j+k])*(descriptors[30*i+k]-centers[30*j+k]);
			}
			if(dist<=best_dist)
			{
				index=j;
				best_dist=dist;
			}
			dist=0;
		}
		best_dist=FLT_MAX;
		nearestLable[i]=index;
	}
	/* level 0 quantization */
	for(i=0;i<length;i++)
	{
		hist[nearestLable[i]]++;
	}
	/* level 1 quantization */
	vector<int> xcell;
	vector<int> ycell;
	for(i=0;i<length;i++)
	{
		xcell.push_back(pointsX[i]/(col/2));
		ycell.push_back(pointsY[i]/(row/2));
	}
	for(i=0;i<length;i++)
	{
		hist[nearestLable[i]+300+NCENTERS*(2*xcell[i]+ycell[i])]++;
	}
	/* level 2 quantization */
	xcell.clear();
	ycell.clear();
	for(i=0;i<length;i++)
	{
		xcell.push_back(pointsX[i]/(col/4));
		ycell.push_back(pointsY[i]/(row/4));
	}
	for(i=0;i<length;i++)
	{
		hist[nearestLable[i]+1500+(4*xcell[i]+ycell[i])*300]++;
	}
	free(nearestLable);
	return NCHANNEL;

}

