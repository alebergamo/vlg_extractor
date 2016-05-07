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
#include "SelfSimDescriptor_fc.h"
#ifdef DEBUG_MISC
#include <sys/time.h>
#endif

SelfSimDescriptor_fc::SelfSimDescriptor_fc()
	{
	    smallSize = DEFAULT_SMALL_SIZE;
	    largeSize = DEFAULT_LARGE_SIZE;
	    numberOfAngles = DEFAULT_NUM_ANGLES;
	    startDistanceBucket = DEFAULT_START_DISTANCE_BUCKET;
	    numberOfDistanceBuckets = DEFAULT_NUM_DISTANCE_BUCKETS;
	}

SelfSimDescriptor_fc::SelfSimDescriptor_fc(int _ssize, int _lsize,
	                                     int _startDistanceBucket,
	                                     int _numberOfDistanceBuckets, int _numberOfAngles)
	{
	    smallSize = _ssize;
	    largeSize = _lsize;
	    startDistanceBucket = _startDistanceBucket;
	    numberOfDistanceBuckets = _numberOfDistanceBuckets;
	    numberOfAngles = _numberOfAngles;
	}

SelfSimDescriptor_fc::SelfSimDescriptor_fc(const SelfSimDescriptor_fc& ss)
	{
	    smallSize = ss.smallSize;
	    largeSize = ss.largeSize;
	    startDistanceBucket = ss.startDistanceBucket;
	    numberOfDistanceBuckets = ss.numberOfDistanceBuckets;
	    numberOfAngles = ss.numberOfAngles;
	}

SelfSimDescriptor_fc::~SelfSimDescriptor_fc()
	{
	}

SelfSimDescriptor_fc& SelfSimDescriptor_fc::operator = (const SelfSimDescriptor_fc& ss)
	{
	    if( this != &ss )
	    {
	        smallSize = ss.smallSize;
	        largeSize = ss.largeSize;
	        startDistanceBucket = ss.startDistanceBucket;
	        numberOfDistanceBuckets = ss.numberOfDistanceBuckets;
	        numberOfAngles = ss.numberOfAngles;
	    }
	    return *this;
	}

size_t SelfSimDescriptor_fc::getDescriptorSize() const
	{
	    return numberOfAngles*(numberOfDistanceBuckets - startDistanceBucket);
	}

Size SelfSimDescriptor_fc::getGridSize( Size imgSize, Size winStride ) const
	{
	    winStride.width = std::max(winStride.width, 1);
	    winStride.height = std::max(winStride.height, 1);
	    int border = largeSize/2 + smallSize/2;
	    return Size(std::max(imgSize.width - border*2 + winStride.width - 1, 0)/winStride.width,
                std::max(imgSize.height - border*2 + winStride.height - 1, 0)/winStride.height);
	}

	// TODO: optimized with SSE2
//modified by Chen Fang, in order to support multi-channel images
void SelfSimDescriptor_fc::SSD(const Mat& img, Point pt, Mat& ssd) const
	{
		uchar* testM = (uchar*)img.datastart;
		uchar* testM1 = testM+1;
		uchar* testM2 = testM1+1;

	    int x, y, dx, dy, r0 = largeSize/2, r1 = smallSize/2;
	    int step = img.step;
	    for( y = -r0; y <= r0; y++ )
	    {
	        float* sptr = ssd.ptr<float>(y+r0) + r0;
	        for( x = -r0; x <= r0; x++ )
	        {
	            int sum = 0;


				const pixelC3* src0 = (pixelC3*) img.ptr<uchar>(y + pt.y - r1) + x + pt.x;
	            const pixelC3* src1 = (pixelC3*) img.ptr<uchar>(pt.y - r1) + pt.x;
	            for( dy = -r1; dy <= r1; dy++, src0 += step/3, src1 += step/3 )
				{
	                for( dx = -r1; dx <= r1; dx++ )
	                {
						int t = (src0[dx].b - src1[dx].b)*(src0[dx].b - src1[dx].b)+(src0[dx].g - src1[dx].g)*(src0[dx].g - src1[dx].g)+(src0[dx].r - src1[dx].r)*(src0[dx].r - src1[dx].r);
	                    sum += t;
	                }
				}
	            sptr[x] = (float)sum;
	        }
	    }
	}


void SelfSimDescriptor_fc::compute(const Mat& img, vector<float>& descriptors, vector<int>& pointsX, vector<int>& pointsY, int* logpolar, Size winStride,
	                                const vector<Point>& locations ) const
	{
	    CV_Assert( img.depth() == CV_8U );

	    winStride.width = std::max(winStride.width, 1);
	    winStride.height = std::max(winStride.height, 1);
	    Size gridSize = getGridSize(img.size(), winStride);
	    int i, nwindows = locations.empty() ? gridSize.width*gridSize.height : (int)locations.size();
	    int border = largeSize/2 + smallSize/2;
	    int fsize = (int)getDescriptorSize();
	    vector<float> tempFeature(fsize+1);
	    descriptors.resize(fsize*nwindows + 1);
	    Mat ssd(largeSize, largeSize, CV_32F); //mappingMask;
	    int index;
	    int offset;
	    float *ssdPtr;
#ifdef DEBUG_MISC
	    double time_total = 0.0;
	    double time0 = 0.0;
	    double time1 = 0.0;
	    double time2 = 0.0;
	    double time3 = 0.0;
	    double time4 = 0.0;
	    double time5 = 0.0;
	    double time_temp = 0.0;
	    timeval t1, t2;
	    timeval t1_total, t2_total;
#endif


	#if 0 //def _OPENMP
	    int nthreads = cvGetNumThreads();
	    #pragma omp parallel for num_threads(nthreads)
	#endif

#ifdef DEBUG_MISC
	    gettimeofday(&t1_total, NULL);
#endif

	    for( i = 0; i < nwindows; i++ )
	    {
	        Point pt;
	        float* feature0 = &descriptors[fsize*i];
	        float* feature = &tempFeature[0];
	        int x, y, j;

#ifdef DEBUG_MISC
	        gettimeofday(&t1, NULL);
#endif
	        if( !locations.empty() )
	        {
	            pt = locations[i];
	            if( pt.x < border || pt.x >= img.cols - border ||
	                pt.y < border || pt.y >= img.rows - border )
	            {
	                for( j = 0; j < fsize; j++ )
	                    feature0[j] = 0.f;
	                continue;
	            }
	        }
	        else
			{
	            pt = Point((i % gridSize.width)*winStride.width + border,
	                       (i / gridSize.width)*winStride.height + border);
				pointsX.push_back(pt.x);
				pointsY.push_back(pt.y);
			}

	       SSD(img, pt, ssd);
#ifdef DEBUG_MISC
	        gettimeofday(&t2, NULL);
	        time_temp = (t2.tv_sec - t1.tv_sec) * 1000.0;
	        time_temp +=  (t2.tv_usec - t1.tv_usec) / 1000.0;
	        time0 = time0 + time_temp;

	            // Determine in the local neighborhood the largest difference and use for normalization

	       gettimeofday(&t1, NULL);
#endif
		   float var_noise = 2700.f;//modified by chen fang, according to lorenzo's classeme project matlab code
	        for( y = -1; y <= 1 ; y++ )
	                    for( x = -1 ; x <= 1 ; x++ )
						{
							var_noise = std::max(var_noise, ssd.at<float>(largeSize/2+y, largeSize/2+x));
							//float test1=ssd.at<float>(0,0);
							//float test2=ssd.at<float>(0,1);
							//float* test3=(float*) ssd.ptr(0);
							//float* test4=((float*) ssd.ptr(0)+1);
							//float* test5=NULL;
						}
#ifdef DEBUG_MISC
	        gettimeofday(&t2, NULL);
	        time_temp = (t2.tv_sec - t1.tv_sec) * 1000.0;
	        time_temp +=  (t2.tv_usec - t1.tv_usec) / 1000.0;
	        time1 = time1 + time_temp;


	        gettimeofday(&t1, NULL);
#endif
	        for( j = 0; j <= fsize; j++ )
	            feature[j] = FLT_MAX;

#ifdef DEBUG_MISC
	        gettimeofday(&t2, NULL);
	        time_temp = (t2.tv_sec - t1.tv_sec) * 1000.0;
	        time_temp +=  (t2.tv_usec - t1.tv_usec) / 1000.0;
	        time2 = time2 + time_temp;

	            // Derive feature vector before exp(-x) computation
	            // Idea: for all  x,a >= 0, a=const.   we have:
	            //       max [ exp( -x / a) ] = exp ( -min(x) / a )
	            // Thus, determine min(ssd) and store in feature[...]

			////modified by Chen Fang
	        gettimeofday(&t1, NULL);
#endif
	        /*for( y = 0; y < ssd.rows; y++ )
	        {
	        	offset = y*largeSize;
				//const schar *mappingMaskPtr = mappingMask.ptr<schar>(y);

	            ssdPtr = ssd.ptr<float>(y);
	            for( x = 0 ; x < ssd.cols; x++ )
	            {
	                index = logpolar[offset+x];
					if((index != -1) && (ssdPtr[x] < feature[index]))
					{
						feature[index] = ssdPtr[x];
					}
				}
	         }*/
	        ssdPtr = ssd.ptr<float>(0);
//#ifdef __GNUC__
//	        __builtin_prefetch(logpolar,0,3);
//	        __builtin_prefetch(feature,1,3);
//#endif
	        for( y = 0; y < (ssd.rows*ssd.cols); y++ )
	        {
	                index = logpolar[y];
					if((index != -1) && (ssdPtr[y] < feature[index]))
					{
						feature[index] = ssdPtr[y];
					}

	         }
#ifdef DEBUG_MISC
	        gettimeofday(&t2, NULL);
	        time_temp = (t2.tv_sec - t1.tv_sec) * 1000.0;
	        time_temp +=  (t2.tv_usec - t1.tv_usec) / 1000.0;
	        time3 = time3 + time_temp;


	        gettimeofday(&t1, NULL);
#endif
	        var_noise = -1.f/var_noise;
			////find the largest feature--modified by Chen Fang////
			float max_ftr = -(FLT_MAX);
	        for( j = 0; j < fsize; j++ )
			{
	            feature0[j] = feature[j]*var_noise;
				if(feature0[j] > max_ftr)
					max_ftr = feature0[j];
			}
#ifdef DEBUG_MISC
	        gettimeofday(&t2, NULL);
	        time_temp = (t2.tv_sec - t1.tv_sec) * 1000.0;
	        time_temp +=  (t2.tv_usec - t1.tv_usec) / 1000.0;
	        time4 = time4 + time_temp;


	        gettimeofday(&t1, NULL);
#endif
	        Mat _f(1, fsize, CV_32F, feature0);
	        cv::exp(_f, _f);
			max_ftr = std::exp(max_ftr);
			//max_ftr = 1/max_ftr;

			/////scale features---modified by Chen Fang/////
			for(j = 0; j < fsize; j++)
			{
				feature0[j] = feature0[j] / max_ftr;
			}
#ifdef DEBUG_MISC
	        gettimeofday(&t2, NULL);
	        time_temp = (t2.tv_sec - t1.tv_sec) * 1000.0;
	        time_temp +=  (t2.tv_usec - t1.tv_usec) / 1000.0;
	        time5 = time5 + time_temp;
#endif

	    }

#ifdef DEBUG_MISC
        gettimeofday(&t2_total, NULL);
        time_temp = (t2_total.tv_sec - t1_total.tv_sec) * 1000.0;
        time_temp +=  (t2_total.tv_usec - t1_total.tv_usec) / 1000.0;
        time_total = time_total + time_temp;

	    printf("\n");
	    printf("time1 %f\n",time1);
	    printf("time2 %f\n",time2);
	    printf("time3 %f\n",time3);
	    printf("time4 %f\n",time4);
	    printf("time5 %f\n",time5);
	    printf("time_total %f\n", time_total);
#endif

	}

/*void SelfSimDescriptor_fc::computeLogPolarMapping(Mat& mappingMask) const
	{
	    mappingMask.create(largeSize, largeSize, CV_8S);

	    // What we want is
	    //           log_m (radius) = numberOfDistanceBuckets
	    //  <==> log_10 (radius) / log_10 (m) = numberOfDistanceBuckets
	    //  <==> log_10 (radius) / numberOfDistanceBuckets = log_10 (m)
	    //  <==> m = 10 ^ log_10(m) = 10 ^ [log_10 (radius) / numberOfDistanceBuckets]
	    //
	    int radius = largeSize/2, angleBucketSize = 360 / numberOfAngles;
	    int fsize = (int)getDescriptorSize();
	    double inv_log10m = (double)numberOfDistanceBuckets/log10((double)radius);

	    for (int y=-radius ; y<=radius ; y++)
	    {
	        schar* mrow = mappingMask.ptr<schar>(y+radius);
	        for (int x=-radius ; x<=radius ; x++)
	        {
	            int index = fsize;
	            float dist = (float)std::sqrt((float)x*x + (float)y*y);
	            int distNo = dist > 0 ? cvRound(log10(dist)*inv_log10m) : 0;
	            if( startDistanceBucket <= distNo && distNo < numberOfDistanceBuckets )
	            {
	                float angle = std::atan2( (float)y, (float)x ) / (float)CV_PI * 180.0f;
	                if (angle < 0) angle += 360.0f;
	                int angleInt = (cvRound(angle) + angleBucketSize/2) % 360;
	                int angleIndex = angleInt / angleBucketSize;
	                index = (distNo-startDistanceBucket)*numberOfAngles + angleIndex;
	            }
	            mrow[x + radius] = saturate_cast<schar>(index);
	        }
	    }
	}*/
