/*
 *  gistGabor.h
 *
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "gistFeature.h"


void gistGabor(const IplImage* img,int Nwindows,IplImage** garbor2D, struct gistFeature* gistfeature);
