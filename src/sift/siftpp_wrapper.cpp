/*
 * siftpp_wrapper.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Alessandro Bergamo
 */

#include <stdio.h>
#include <iostream>
#include "sift.hpp"
#include "siftpp_wrapper.h"

using namespace std;


void siftpp_wrapper_test() {
	printf("this is a test\n");
}

int compute_sift_descriptors(unsigned char* img_data, int img_width, int img_height, int* n_descriptors, double** descriptors) {

	int    first          = -1 ;
	int    octaves        = -1 ;
	int    levels         = 3 ;
	float  threshold      = 0.04f / levels / 2.0f ;
	float  edgeThreshold  = 10.0f;
	int    verbose        = 1 ;
	int    unnormalized   = 0 ;
	float  magnif         = 3.0 ;
	int    nodescr        = 0;
	int    noorient       = 0 ;

		// output image: outputFilename = std::string(optarg);
		// input image name(argv[0]);

	// create the PgbBuffer, that is our image
	int img_n_pixels = img_width*img_height;
	VL::PgmBuffer buffer;
	buffer.width = img_width;
	buffer.height = img_height;
	buffer.data = new VL::pixel_t[img_n_pixels];
	assert(buffer.data);
	for (int i=0; i < img_n_pixels; ++i) {
		buffer.data[i] = static_cast<float>(img_data[i]) / 255.0f; // normalize to 1 the values
	}

#ifdef DEBUG
	cout << "img_data ";
	for (int i=0; i < 20; ++i) {
		cout << (int) img_data[i*100] << " ";
	}
	cout << endl;

	cout << "### PgbBuffer ###" << endl;
	cout << "buffer.width " << buffer.width << endl;
	cout << "buffer.height" << buffer.height << endl;
	cout << "buffer.data ";
	for (int i=0; i < 20; ++i) {
		cout << buffer.data[i*100] << " ";
	}
	cout << endl;
#endif


	// ---------------------------------------------------------------
	//                                            Gaussian scale space
	// ---------------------------------------------------------------
#ifdef DEBUG
	verbose && cout << "siftpp: computing Gaussian scale space" << endl;
#endif

	int O = octaves;
	int const S = levels;
	int const omin = first;
	float const sigman = .5;
	float const sigma0 = 1.6 * powf(2.0f, 1.0f / S);

	// optionally autoselect the number number of octaves
	// we downsample up to 8x8 patches
	if (O < 1) {
		O = std::max(int(std::floor(  log((double) std::min(buffer.width, buffer.height)) / log(2.0)  ) - omin - 3), 1);
	}

#ifdef DEBUG
    verbose && cout
      << "siftpp:   number of octaves     : " << O << endl
      << "siftpp:   first octave          : " << omin << endl
      << "siftpp:   levels per octave     : " << S << endl
      << "sigman " << sigman << endl
      << "sigma0 " << sigma0 << endl
      << "O " << O << endl
      << "S " << S << endl
      << "omin " << omin << endl
      << endl ;

	cout << "buffer.data ";
	for (int i=0; i < 20; ++i) {
		cout << buffer.data[i*100] << " ";
	}
	cout << endl;
#endif

	// initialize scalespace
	VL::Sift sift(buffer.data, buffer.width, buffer.height, sigman, sigma0, O, S, omin, -1, S + 1);

#ifdef DEBUG
	verbose && cout << "siftpp: Gaussian scale space completed" << endl;
#endif

	// -------------------------------------------------------------
	//                                             Run SIFT detector
	// -------------------------------------------------------------
#ifdef DEBUG
	verbose && cout << "siftpp: running detector  " << endl
			<< "siftpp:   threshold             : " << threshold
			<< endl << "siftpp:   edge-threshold        : "
			<< edgeThreshold << endl;
#endif

	sift.detectKeypoints(threshold, edgeThreshold);

#ifdef DEBUG
	verbose && cout << "siftpp: detector completed with "
			<< sift.keypointsEnd() - sift.keypointsBegin()
			<< " keypoints" << endl;
#endif

	// -------------------------------------------------------------
	//                  Run SIFT orientation detector and descriptor
	// -------------------------------------------------------------

	/* set descriptor options */
	sift.setNormalizeDescriptor(!unnormalized);
	sift.setMagnification(magnif);

#ifdef DEBUG
	if (verbose) {
		cout << "siftpp: ";
		if (!noorient & nodescr)
			cout << "computing keypoint orientations";
		if (noorient & !nodescr)
			cout << "computing keypoint descriptors";
		if (!noorient & !nodescr)
			cout << "computing orientations and descriptors";
		if (noorient & nodescr)
			cout << "finalizing";
		cout << endl;
	}
#endif

	// -------------------------------------------------------------
	//            Run detector, compute orientations and descriptors
	// -------------------------------------------------------------
	vector<VL::float_t *> descr_v;
	vector<VL::float_t> desc_x_v;
	vector<VL::float_t> desc_y_v;
	for (VL::Sift::KeypointsConstIter iter = sift.keypointsBegin(); iter != sift.keypointsEnd(); ++iter) {

		// detect orientations
		VL::float_t angles[4];
		int nangles;
		if (!noorient) {
			nangles = sift.computeKeypointOrientations(angles, *iter);
		} else {
			nangles = 1;
			angles[0] = VL::float_t(0);
		}

		// compute descriptors
		for (int a = 0; a < nangles; ++a) {

#ifdef DEBUG
			cout  << iter->x << ' '
					<< iter->y << ' '
					<< iter->sigma << ' '
					<< angles[a] << "----- [  " ;
#endif

			/* compute descriptor */
			VL::float_t *descr_pt = new VL::float_t[128];
			sift.computeKeypointDescriptor(descr_pt, *iter, angles[a]);

#ifdef DEBUG
			cout << descr_pt[0] << ' ' << descr_pt[1] << ' ' << descr_pt[2] << " .... ]";
#endif

			/* save descriptor to to appropriate list */
			descr_v.push_back(descr_pt);
			desc_x_v.push_back(iter->x);
			desc_y_v.push_back(iter->y);

			/* next line */
#ifdef DEBUG
			 cout << endl;
#endif


		} // next angle
	} // next keypoint


	// -------------------------------------------------------
	//         Return the values
	// -------------------------------------------------------
	if (descr_v.size() <= 0) {
		delete[] buffer.data;
		return -100;
	}

	*n_descriptors = descr_v.size();
	*descriptors = (double*) malloc((*n_descriptors)*130*sizeof(double));
	double* desc_pointer = *descriptors;
	if (! desc_pointer) {
		printf("ERROR MEMORY!!!!\n");
		exit(-1);
	}
	for (unsigned int i=0; i < descr_v.size(); ++i) {
		*desc_pointer = static_cast<double>(desc_x_v[i]);
		++desc_pointer;
		*desc_pointer = static_cast<double>(desc_y_v[i]);
		++desc_pointer;

		for (int j=0; j < 128; ++j) {
			*desc_pointer = static_cast<double>(descr_v[i][j]);
			++desc_pointer;
		}
		delete[] descr_v[i];
	}
	// free
	delete[] buffer.data;

	return 0;
}
