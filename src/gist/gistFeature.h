/*
 *  gistFeature.h
 *
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#ifndef GISTFEATURE_H
#define GISTFEATURE_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define NWINDOWS 4
#define CHANNEL 3
#define NFILTERS 20


struct gistFeature
{
	int Nfilters;
	int Nwindows;
	int Channels;
	float features[NWINDOWS*NWINDOWS*CHANNEL*NFILTERS];
	/*features are stored in an array according to the following format
		
		            Filter 1                           Filter 2               .................          Filter n
		Channel 1    ...   Channel n          Channel 1    ...   Channel n                     Channel 1    ...   Channel n
		W1 ... Wn          W1 ... Wn          W1 ... Wn          W1 ... Wn                     W1 ... Wn          W1 ... Wn
	*/
};

bool createGistFeature(struct gistFeature* gistfeature);


/*get a feature of specific filter number, window number and channel number*/
bool getFeature(float* feature, const struct gistFeature* gistfeature, int filter, int window, int channel);//filters,windows,channels all start from zero

void freeGist(struct gistFeature* gist);

#endif
