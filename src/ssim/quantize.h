/*
 *  quantize.h
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef QUANTIZE_H
#define QUANTIZE_H

#include "stdio.h"
#include <vector>
#include "float.h"

using namespace std;

#define CDIM 30
#define NCENTERS 300
#define NCHANNEL 6300


int quantize(vector<float> descriptors, vector<int> pointsX, vector<int> pointsY,int* & hist,float* centers,int row, int col);

#endif
