/*
 *  gistFeature.c
 *
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include "gistFeature.h"

#define NWINDOWS 4
#define CHANNEL 3
#define NFILTERS 20

bool createGistFeature(struct gistFeature* gistfeature)
{
	gistfeature->Nfilters=NFILTERS;
	gistfeature->Nwindows=NWINDOWS;
	gistfeature->Channels=CHANNEL;
	return true;
}

/*get a feature of specific filter number, window number and channel number*/
bool getFeature(float* feature,const struct gistFeature* gistfeature,int filter,int window,int channel)//filters,windows,channels all start from zero
{
	int nfilters=gistfeature->Nfilters;
	int nwindows=gistfeature->Nwindows;
	int nchannels=gistfeature->Channels;
	if(filter>nfilters||window>nwindows||channel>nchannels)
		return false;
	else
	{
		*feature=gistfeature->features[nwindows*nchannels*filter+nwindows*channel+window];
		return true;
	}
}
