// This is based on Rainer Lienhart contribution. Below is the original copyright:
//
/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                University of Augsburg License Agreement
//                For Open Source MultiMedia Computing (MMC) Library
//
// Copyright (C) 2007, University of Augsburg, Germany, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of University of Augsburg, Germany may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the University of Augsburg, Germany or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//   Author:    Rainer Lienhart
//              email: Rainer.Lienhart@informatik.uni-augsburg.de
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Please cite the following two papers:
// 1. Shechtman, E., Irani, M.:
//              Matching local self-similarities across images and videos.
//              CVPR, (2007)
// 2. Eva Horster, Thomas Greif, Rainer Lienhart, Malcolm Slaney.
//              Comparing Local Feature Descriptors in pLSA-Based Image Models.
//              30th Annual Symposium of the German Association for
//      Pattern Recognition (DAGM) 2008, Munich, Germany, June 2008.

/*
 *  SelfSimDescriptor_fc.h
 *  chenfang@cs.dartmouth.edu
 *  This code is modified by Chen Fang
 */

#ifndef SELFSIMDESCRIPTOR_FC_H
#define SELFSIMDESCRIPTOR_FC_H

#include <opencv/cv.h>
#include <opencv/cv.hpp>
//#include <cvaux.h>
//#include <cvaux.hpp>
#include <cxcore.h>
#include <cxcore.hpp>
#include <opencv/highgui.h>


using namespace cv;


struct pixelC3
{
	uchar r;
	uchar g;
	uchar b;
};

class SelfSimDescriptor_fc
{
public:
    SelfSimDescriptor_fc();
    SelfSimDescriptor_fc(int _ssize, int _lsize,
        int _startDistanceBucket=DEFAULT_START_DISTANCE_BUCKET,
        int _numberOfDistanceBuckets=DEFAULT_NUM_DISTANCE_BUCKETS,
        int _nangles=DEFAULT_NUM_ANGLES);
	SelfSimDescriptor_fc(const SelfSimDescriptor_fc& ss);
	virtual ~SelfSimDescriptor_fc();
    SelfSimDescriptor_fc& operator = (const SelfSimDescriptor_fc& ss);

    size_t getDescriptorSize() const;
    Size getGridSize( Size imgsize, Size winStride ) const;

    virtual void compute(const Mat& img, vector<float>& descriptors,vector<int>& pointsX, vector<int>& pointsY, int* logpolar, Size winStride=Size(),const vector<Point>& locations=vector<Point>()) const;
    //virtual void computeLogPolarMapping(Mat& mappingMask) const;
    virtual void SSD(const Mat& img, Point pt, Mat& ssd) const;

	int smallSize;
	int largeSize;
    int startDistanceBucket;
    int numberOfDistanceBuckets;
    int numberOfAngles;

    enum { DEFAULT_SMALL_SIZE = 5, DEFAULT_LARGE_SIZE = 41,
        DEFAULT_NUM_ANGLES = 20, DEFAULT_START_DISTANCE_BUCKET = 3,
        DEFAULT_NUM_DISTANCE_BUCKETS = 7 };
};
	

#endif
