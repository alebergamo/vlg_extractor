/*
 * extract_gist.c
 *
 *  Copyright 2011    Author: Chen Fang
 *  chenfang@cs.dartmouth.edu
 */

#include "extract_gist.h"
#include "makeGabor.h"
#include "makePrefilter.h"
#include "prefilt.h"
#include "gistGabor.h"
#include "gistFeature.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <time.h>


int extract_gist( IplImage* img, IplImage* prefilter, IplImage** gabor,CvMat* gistM )
// img: input image
// prefilter: prefilter read from disk, namely, the output of makePrefilter
// gabor: gabor filter read from disk, namely, the output of makeGabor
// gistM: output features
{
	if((!img) || (!prefilter) || (!gabor) || (!gistM))
		return ERROR_GIST_EXTRACTION_PROBLEM;
	int i=0;
	
	IplImage* convImg=cvCreateImage(cvSize(GIST_IMGSIZE,GIST_IMGSIZE),IPL_DEPTH_32F,3);
	IplImage* prefedImg=cvCreateImage(cvSize(GIST_IMGSIZE,GIST_IMGSIZE),IPL_DEPTH_32F,3);

	struct gistFeature gist;
	createGistFeature(&gist);

	IplImage* resizeImg=cvCreateImage(cvSize(GIST_IMGSIZE,GIST_IMGSIZE),img->depth,3);
	//cvResize(img,resizeImg,CV_INTER_LINEAR);/* resize the image */
	cvResize(img,resizeImg,CV_INTER_AREA);/* resize the image */
	cvConvertScale(resizeImg,convImg,1,0);/* convert the data type */

	prefilt(convImg,prefedImg,5,prefilter);
	gistGabor(prefedImg,GIST_NWINDOWS,gabor,&gist);

	memcpy((void*)gistM->data.fl,(void*)gist.features,sizeof(float)*GIST_NWINDOWS*GIST_NWINDOWS*GIST_NFILTERS*GIST_CHANNEL);
	cvReleaseImage(&convImg);
	cvReleaseImage(&prefedImg);
	cvReleaseImage(&resizeImg);

	return 0;
}
