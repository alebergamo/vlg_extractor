/***************************************************************************
 *            sift.c
 *
 *  Fri Dec 17 16:07:01 2010
 *
 *  Copyright  2010  Alessandro Bergamo, Chen Fang
 *  <aleb@cs.dartmouth.edu>
 *  <chen@cs.dartmouth.edu>
 ****************************************************************************/

#include "sift2.h"
#include "sift_center.h"
#include <float.h>
#include <math.h>

/*
 * centers is the vector of centers for the BOW
 * If centers==NULL, we do NOT quantize the interest points. Instead 'feature' will be the the interest points matrix,
 * organized as a vector, as following:
 * [[<image width> <image height> <x position interest point> <y position> <128 dims vector>] , [2nd descriptor], ..., [n_descriptors'th descriptor]]
 * */
int sift_extraction(IplImage* img_original, Matrix* feature, float* centers) {
	IplImage* img_gray = NULL; /* gray scale image*/
	CvMat* img_gray_mat = NULL; /* gray scale image in CvMat format */
	CvMat* img_gray_resized_mat = NULL;
	CvMat* img_gray_normalized_mat = NULL;
	int img_resized_width; /* the width (in pixels) of the final resized image */
	int img_resized_height; /* the height (in pixels) of the final resized image */
	double scale_factor = 1.0; /* scale factor */
	int res;
#ifdef DEBUG_MISC
	char buffer[5000];
#endif

	int i = 0;
	int j = 0;
	int img_hist[256]; /* the gray-scale-version image histogram */
	int total_pixels = INT_MIN; /* the number of pixels of the original image */
	int hist_min_value = INT_MAX; /* the minimum non-zero bin of the hist (thus, closest to the black) */
	int hist_max_value = INT_MIN; /* the maximum non-zero bin of the hist (thus, closest to the white) */
	int bin_to_white = 0; /* the starting bin to push on the white values */
	int bin_to_black = 0;
	int n_pixels_to_white = 0; /* number of pixels to push on white (it is the right cumulative of the histogram) */
	int n_pixels_to_black = 0;
	double contrast_alpha = 0; /* contrast function is defined as p' = p*alpha + beta */
	double contrast_beta = 0;
	int bin_target_white = 255; /* in some cases it is better to do *not* push the values to an absolute white, but a lower value */
	int bin_target_black = 0;
	int pixel; /* temporary value */

	int n_descriptors;  /* number of SIFT descriptors */
	double* descriptors; /* array, with all the descriptors concatenated */
	double* descriptors_temp;
	Matrix* descriptors_matrix = NULL;  /* the Matrix descriptors [N * 128] */


	/* convert the image in grey-levels */
	img_gray = cvCreateImage(cvSize(img_original->width, img_original->height ), IPL_DEPTH_8U, 1);
	cvCvtColor(img_original, img_gray, CV_RGB2GRAY);

	/* convert the image to a cvMat */
	img_gray_mat = cvCreateMat(img_gray->height, img_gray->width, CV_8UC1);
	cvConvert(img_gray, img_gray_mat);

	/*P.S.: original preprocessing pipeline
	cmdstr = ['!djpeg "' imgfolder imgname '"  | ppmtopgm | pnmnorm -bpercent=0.01 -wpercent=0.01 -maxexpand=400 | pamscale -pixels 786432 > "' tmpfile '"']; */

	/* calculate the histogram */
	total_pixels = img_gray_mat->cols * img_gray_mat->rows;
	for (i=0; i < 256; ++i) {
		img_hist[i] = 0;
	}
	for (i=0; i < total_pixels; ++i) {
		++img_hist[(unsigned char) img_gray_mat->data.ptr[i]];
	}
	for (i=0; i < 256; ++i) {
		if (img_hist[i] > 0) {
			if (i > hist_max_value) {
				hist_max_value = i;
			}
			if (i < hist_min_value) {
				hist_min_value = i;
			}
		}
	}

	/* calculate the 0.01 percent bins for the white pixels*/
	bin_to_white = hist_max_value;
	n_pixels_to_white = img_hist[bin_to_white];
	while (((float)n_pixels_to_white / total_pixels) < WPERCENT) {
		--bin_to_white;
		n_pixels_to_white = n_pixels_to_white + img_hist[bin_to_white];
	}
	/* bpercent */
	bin_to_black = hist_min_value;
	n_pixels_to_black = img_hist[bin_to_black];
	while (((float)n_pixels_to_black / total_pixels) < BPERCENT) {
		++bin_to_black;
		n_pixels_to_black = n_pixels_to_black + img_hist[bin_to_black];
	}
	/* calculate the transformation. If the stretching > 400% then reduce it */
	bin_target_white = 256;
	bin_target_black = -1;
	do {
		--bin_target_white;
		++bin_target_black;
		contrast_alpha = (bin_target_black - bin_target_white) / (double)(bin_to_black - bin_to_white);
		contrast_beta = bin_target_black - (double)contrast_alpha*bin_to_black;
	} while ((contrast_alpha*bin_to_white - contrast_alpha*bin_to_black + 1) / (bin_to_white - bin_to_black + 1) > MAXEXPAND);
	/* normalize the image */
	img_gray_normalized_mat = cvCreateMat(img_gray_mat->rows, img_gray_mat->cols, CV_8UC1);
	for (i=0; i < total_pixels; ++i) {
		pixel = (int) (contrast_alpha*((unsigned char)img_gray_mat->data.ptr[i])+contrast_beta);
		if (pixel > bin_target_white) {
			pixel = bin_target_white;
		}
		if (pixel < bin_target_black) {
			pixel = bin_target_black;
		}
		img_gray_normalized_mat->data.ptr[i] = (unsigned char) pixel;
	}

	/* resize the image, m with the constraint maxpixels=786432 */
	scale_factor = sqrt(786432 / (double)(img_gray_normalized_mat->rows * img_gray_normalized_mat->cols));
	if (scale_factor >= 1.0) {
		scale_factor = 1.0; /* if number of original pixels <= 786432 we do not increase the size */
	}
	img_resized_width = scale_factor * img_gray_normalized_mat->cols;
	img_resized_height = scale_factor * img_gray_normalized_mat->rows;
	img_gray_resized_mat = cvCreateMat(img_resized_height, img_resized_width, CV_8UC1);
	cvResize(img_gray_normalized_mat, img_gray_resized_mat, CV_INTER_LINEAR);


	/* DEBUG: save the file */
#ifdef DEBUG_MISC
	sprintf(buffer, "/tmp/resized.pgm");
	cvSaveImage(buffer, img_gray_resized_mat, CV_LOAD_IMAGE_GRAYSCALE);

	printf("img_gray_resized_mat->rows: %d\n", img_gray_resized_mat->rows);
	printf("img_gray_resized_mat->cols: %d\n", img_gray_resized_mat->cols);
	printf("img_gray_resized_mat->data.ptr: ");
	for (i=0; i < 20; ++i) {
		printf("%u ", (unsigned char) (img_gray_resized_mat->data.ptr[i*100]) );
	}
	printf("\n");
#endif

	/*  ***** SIFT extraction **** */
	res = compute_sift_descriptors(img_gray_resized_mat->data.ptr, img_gray_resized_mat->cols, img_gray_resized_mat->rows, &n_descriptors, &descriptors);
	if (res == -100) {
		/* In this case the SIFT extractor simply didn't find and interest-point.
		 * Instead of return an error, we just return a zero-vector (i.e. there are no BOWs present) */
		cvReleaseMat(&img_gray_resized_mat);
		cvReleaseMat(&img_gray_normalized_mat);
		cvReleaseMat(&img_gray_mat);
		cvReleaseImage(&img_gray);

		create_matrix_zero(feature, HISTO_SIZE, 1);

		return 0;
	} else if (res != 0) {
		cvReleaseMat(&img_gray_resized_mat);
		cvReleaseMat(&img_gray_normalized_mat);
		cvReleaseMat(&img_gray_mat);
		cvReleaseImage(&img_gray);

		return ERROR_SIFT_EXTRACTION_FAIL;
	}

	/* construct the interest points matrix
	 * The matrix is organized as a vector, organized as follow:
	 * [[<image width> <image height> <x position interest point> <y position> <128 dims vector>] , [2nd descriptor], ..., [n_descriptors'th descriptor]]
	 * */
	descriptors_temp = descriptors;
	descriptors_matrix = (Matrix*) malloc(sizeof(Matrix));
	if(!n_descriptors) {
		return ERROR_SIFT_EXTRACTION_FAIL;
	}
	create_matrix(descriptors_matrix, n_descriptors, 132);
	for (i=0; i < n_descriptors; ++i) {
		matrix_set(descriptors_matrix, i, 0, (float) img_gray_resized_mat->cols);
		matrix_set(descriptors_matrix, i, 1, (float) img_gray_resized_mat->rows);

		for (j=2; j < 132; ++j) {
			matrix_set(descriptors_matrix, i, j, (float) *descriptors_temp);
			++descriptors_temp;
		}
	}

	/* if we do not need the quantization part, let's just exit */
	if (!centers) {
		/* 'feature' will be just a clone of 'descriptors_matrix' */
		matrix_clone2(feature, descriptors_matrix);

		cvReleaseMat(&img_gray_resized_mat);
		cvReleaseMat(&img_gray_normalized_mat);
		cvReleaseMat(&img_gray_mat);
		cvReleaseImage(&img_gray);
		delete_matrix(descriptors_matrix);
		free(descriptors_matrix);
		free(descriptors);

		return 0;
	}


	/*************quantization*************/
	/*find the nearest cluster center*/
	int ndesc = descriptors_matrix->n_rows;
	int dim_desc = descriptors_matrix->n_cols - 4;
	int imgx_width = matrix_get(descriptors_matrix,0,0);
	int imgy_height = matrix_get(descriptors_matrix,0,1);
	float* cur_row = NULL;
	int* nearesttable = (int*) malloc(sizeof(int)*ndesc);
	float* cur_desc = (float*) malloc(sizeof(float)*dim_desc);
	int k;
	for(i=0; i<ndesc; i++)
	{
		/***calculate the nearest cluster center and update the nearesttable***/
		cur_row = descriptors_matrix->data + 4 + i * descriptors_matrix->n_cols;
		memcpy(cur_desc, cur_row, sizeof(float)*dim_desc);
		float best_dist = FLT_MAX;
		float dist = 0;
		int index = 0;
		for(j=0; j<NSIFTCENTER; j++)
		{
			for(k=0; k<dim_desc; k++)
			{
				dist = dist + (cur_desc[k]-centers[k + dim_desc*j]) * (cur_desc[k]-centers[k + dim_desc*j]);//L2 distance
			}
			if(dist < best_dist)
			{
				best_dist = dist;
				/*update the nearest index*/
				index = j;
			}
			dist = 0;
		}
		nearesttable[i] = index;
	}



	/* construct the final histogram */
	Matrix* histo = feature;
	//create_matrix_zero(histo, 1, HISTO_SIZE);
	create_matrix_zero(histo, HISTO_SIZE, 1);
	float* histo_ptr = histo->data;
	/* level 0*/
	for(i=0; i<ndesc; i++)
	{
		histo_ptr[nearesttable[i]]++;
	}
	/* get location of interest points */
	int* pointx = (int*) malloc(sizeof(int)*ndesc);
	int* pointy = (int*) malloc(sizeof(int)*ndesc);
	for(i=0; i<ndesc; i++)
	{
		pointx[i] = floor(matrix_get(descriptors_matrix,i,2));
		pointy[i] = floor(matrix_get(descriptors_matrix,i,3));
	}
	/* level 1 & level 2 */
	int* cellx = (int*) malloc(sizeof(int)*ndesc);
	int* celly = (int*) malloc(sizeof(int)*ndesc);
	for(i=0; i<HISTO_L; i++)
	{
		for(j=0; j<ndesc; j++)
		{
			cellx[j] = pointx[j] / (imgx_width/pow((float)2,i+1));
			celly[j] = pointy[j] / (imgy_height/pow((float)2,i+1));

			/* test code starts */
			if(i==0 && (cellx[j]>=2 || celly[j]>=2))
				printf("L=1,cellx and celly out of bound");
			if(i==1 && (cellx[j]>=4 || celly[j]>=4))
				printf("L=2,cellx and celly out of bound");
			/* test code ends */
		}
		int base = NSIFTCENTER * ((int)pow((float)4,i+1)-1)/3;
		int cursor = base;
		for(j=0; j<ndesc; j++)
		{
			cursor = base+(pow((float)2,i+1)*cellx[j]+celly[j])*NSIFTCENTER+nearesttable[j];
			histo_ptr[cursor]++;
		}
	}






	/* free memory */
	cvReleaseMat(&img_gray_resized_mat);
	cvReleaseMat(&img_gray_normalized_mat);
	cvReleaseMat(&img_gray_mat);
	cvReleaseImage(&img_gray);
	delete_matrix(descriptors_matrix);
	free(descriptors_matrix);
	free(descriptors);
	free(nearesttable);
	free(cur_desc);
	free(pointx);
	free(pointy);
	free(cellx);
	free(celly);

	return 0;
}
