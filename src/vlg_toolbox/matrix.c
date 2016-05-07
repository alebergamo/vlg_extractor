/***************************************************************************
 *            matrix.c
 *
 *  Mon Jul 26 15:09:13 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <string.h>

#ifdef MAT_SUPPORT
#include <mat.h>
#endif

int create_matrix(Matrix* M, unsigned int n_rows, unsigned int n_cols) {
	assert(M);
	assert(n_rows > 0);
	assert(n_cols > 0);

	M->data = (float*) malloc(n_rows*n_cols*sizeof(float));
	if (!M->data) {
		exit(ERROR_MEM);
		return ERROR_MEM;
	}

	M->n_rows = n_rows;
	M->n_cols = n_cols;
	M->n_elements = n_rows*n_cols;

	return 0;
}

int create_matrix_zero(Matrix* M, unsigned int n_rows, unsigned int n_cols) {
	unsigned int i;
	int res;

	assert(M);
	assert(n_rows > 0);
	assert(n_cols > 0);

	res = create_matrix(M, n_rows, n_cols);
	if (res) {
		return res;
	}

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (float) 0.0;
	}

	return 0;
}

int create_matrix_initialized(Matrix* M, float* data, unsigned int n_rows, unsigned int n_cols)
{
	assert(M);
	assert(data);
	assert(n_rows > 0);
	assert(n_cols > 0);

	M->data = data;
	M->n_rows = n_rows;
	M->n_cols = n_cols;
	M->n_elements = n_rows*n_cols;

	return 0;
}

int create_matrix_random(Matrix* M, unsigned int n_rows, unsigned int n_cols)
{
	unsigned int i;
	int res;

	assert(M);
	assert(n_rows > 0);
	assert(n_cols > 0);

	res = create_matrix(M, n_rows, n_cols);
	if (res) {
		return res;
	}

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (float)rand() / RAND_MAX;
	}

	return 0;
}

int delete_matrix(Matrix* M) {
	assert(M);

	free(M->data);

	return 0;
}


int read_ascii_matrix(Matrix* M, const char* file_name, unsigned int n_rows, unsigned int n_cols)
{
	FILE* fin = NULL;
	unsigned int i,j;
	float temp;
	int res;

	assert(file_name);
	assert(M);
	assert(n_rows > 0);
	assert(n_cols > 0);

	fin = fopen(file_name, "r");
	if (!fin) {
		return ERROR_IO;
	}

	res = create_matrix(M, n_rows, n_cols);
	if (res) {
		return res; /*ERROR_MEM*/
	}

	for (i=0; i < M->n_rows; ++i) {
		for (j=0; j < M->n_cols; ++j) {
			fscanf(fin, "%f", &temp);
			M->data[i*M->n_cols+j] = temp;
		}
	}

	res = fclose(fin);
	assert(!res);

	return 0;
}

int write_ascii_matrix(const Matrix* M, const char* file_name)
{
	FILE* fout = NULL;
	unsigned int i,j;
	int res;

	assert(file_name);
	assert(M);

	fout = fopen(file_name, "w");
	if (!fout) {
		return ERROR_IO;
	}

	for (i=0; i < M->n_rows; ++i) {
		for (j=0; j < M->n_cols; ++j) {
			fprintf(fout, "%e ", M->data[i*M->n_cols+j]);
		}
		fprintf(fout, "\n");
	}

	res = fclose(fout);
	assert(!res);

	return 0;
}


int read_float_matrix(Matrix* M, const char* file_name)
{
	FILE* fin = NULL;
	unsigned int temp_uint;
	size_t res;
	int res2;

	assert(file_name);
	assert(M);

	/* open the file */
	fin = fopen(file_name, "rb");
	if (!fin) {
		return ERROR_IO;
	}
	/* read n_rows,n_cols*/
	res = fread(&temp_uint, sizeof(unsigned int), 1, fin);
	assert(res > 0);
	M->n_rows = temp_uint;
	res = fread(&temp_uint, sizeof(unsigned int), 1, fin);
	assert(res > 0);
	M->n_cols = temp_uint;
	/* create the Matrix */
	res2 = create_matrix(M, M->n_rows, M->n_cols);
	if (res2) {
		fclose(fin);
		return res2;
	}
	/* copy the data */
	res = fread(M->data, sizeof(float), M->n_elements, fin);
	if(res != M->n_elements) {
		return ERROR_IO;
	}
	/* close the file and exit */
	res2 = fclose(fin);
	assert(!res2);

	return 0;
}

int write_float_matrix(const Matrix* M, const char* file_name)
{
	FILE* fout = NULL;
	size_t res;
	int res2;

	assert(file_name);
	assert(M);

	/* open the file */
	fout = fopen(file_name, "wb");
	if (!fout) {
		return ERROR_IO;
	}
	/* write n_rows,n_cols,data */
	res = fwrite(&M->n_rows, sizeof(unsigned int), 1, fout);
	assert(res > 0);
	res = fwrite(&M->n_cols, sizeof(unsigned int), 1, fout);
	assert(res > 0);
	res = fwrite(M->data, sizeof(float), M->n_elements, fout);
	assert(res == M->n_elements);
	/* close the file and return */
	res2 = fclose(fout);
	assert(!res2);

	return 0;
}

int write_bit_vector(const Matrix* v, const char* file_name)
{
	FILE* fout = NULL;
	size_t res;
	unsigned int i;
	unsigned int idx_byte;
	unsigned int idx_bit; /* 0..CHAR_BIT-1, that is 0..7 */
	unsigned char* v_bit = NULL;
	unsigned int n_bytes;
	int res2;

	assert(file_name);
	assert(v);
	assert(v->n_cols==1 || v->n_rows==1);

	/* open the file */
	fout = fopen(file_name, "wb");
	if (!fout) {
		return ERROR_IO;
	}
	/* convert the data in the bit-vector v_bin */
	if ((v->n_elements % CHAR_BIT) == 0) {
		n_bytes = (v->n_elements/CHAR_BIT);
	} else {
		n_bytes = ceil((double)(v->n_elements/CHAR_BIT))+1; // TODO the ceil is not necessary
	}
	v_bit = (unsigned char*) calloc(n_bytes, sizeof(unsigned char));
	if (!v_bit) {
		exit(ERROR_MEM);
	}
	idx_byte= 0;
	idx_bit = 0;
	for (i=0; i < v->n_elements; ++i) {
		if (v->data[i] > 0) {
			idx_byte = i / CHAR_BIT;
			idx_bit = i % CHAR_BIT;
			v_bit[idx_byte] = v_bit[idx_byte] | (unsigned char)(pow(2.0, (double)(CHAR_BIT-1-idx_bit))); // TODO: pow???? Implement as a bit-shift
		}
	}
	/* write the vector of bits */
	res = fwrite(v_bit, sizeof(unsigned char), n_bytes, fout);
	assert(res == n_bytes);
	/* free, close the file and return */
	free(v_bit);
	res2 = fclose(fout);
	assert(!res2);

	return 0;
}


void print_ascii_matrix(const Matrix* M)
{
	unsigned int i,j;

	assert(M);

	for (i=0; i < M->n_rows; ++i) {
		for (j=0; j < M->n_cols; ++j) {
			printf("%f ", M->data[i*M->n_cols+j]);
		}
		printf("\n");
	}
	printf("\n");
}

void matrix_op_sin(Matrix* M)
{
	unsigned int i;

	assert(M);

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (float) sin(M->data[i]);
	}
}

void matrix_op_cos(Matrix* M)
{
	unsigned int i;

	assert(M);

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (float) cos(M->data[i]);
	}
}

void matrix_op_log(Matrix* M)
{
	unsigned int i;

	assert(M);

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (float) log(M->data[i]);
	}
}

void matrix_op_sqrt(Matrix* M)
{
	unsigned int i;

	assert(M);

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (float) sqrt(M->data[i]);
	}
}

void matrix_op_binarize(Matrix* M, float threshold)
{
	unsigned int i;

	assert(M);

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = (M->data[i] > threshold) ? 1.0 : 0.0;
	}
}

void matrix_op_divide(Matrix* M, float c)
{
	unsigned int i;

	assert(M);

	for (i=0; i < M->n_elements; ++i) {
		M->data[i] = M->data[i] / c;
	}
}

void scalar_times_matrix(Matrix* M, float alpha)
{
	/*double* y;*/
	unsigned int i;

	assert(M);

	/* v1 */
	/*y = calloc(M->n_elements, sizeof(double));
	cblas_daxpy(M->n_elements, alpha, M->data, 1, y, 1);
	free(y);*/

	/* v2 */
	for (i=0; i < M->n_elements; ++i) {
	  M->data[i] = M->data[i] * alpha;
	}
}

Matrix* rep_vect(const Matrix* M, unsigned int y, unsigned int x)
{
	unsigned int i,idx_x,idx_y;
	Matrix* Mout = NULL;

	assert(M);
	assert(y > 0);
	assert(x > 0);
	assert(!(M->n_rows > 1) || !(M->n_cols > 1));

	Mout = (Matrix*) malloc(sizeof(Matrix));
	if (!Mout) {
		exit(ERROR_MEM);
	}
	create_matrix(Mout, M->n_rows*y, M->n_cols*x);
	if (M->n_cols == 1) {
		/* M is a column-vector */
		for (idx_x=0; idx_x < x; ++idx_x) {
			for(idx_y=0; idx_y < y; ++idx_y) {
				for (i=0; i < M->n_elements; ++i) {
					Mout->data[i*x+idx_x+idx_y*x*M->n_elements] = M->data[i];
				}
			}
		}
	} else {
		/* M is a row-vector */
		for (i=0; i < x*y; ++i) {
			memcpy( Mout->data + i*M->n_elements, M->data, M->n_elements*sizeof(double));
		}
	}

	return Mout;
}

void matrix_times_matrix_element(Matrix* M1, const Matrix* M2)
{
  unsigned int i;

	assert(M1);
	assert(M2);
	assert((M1->n_cols == M2->n_cols) && (M1->n_rows == M2->n_rows));

  for (i=0; i < M1->n_elements; ++i) {
	M1->data[i] = M1->data[i] * M2->data[i];
  }
}

void matrix_plus_matrix(Matrix* M1, const Matrix* M2)
{
	unsigned int i;

	assert(M1);
	assert(M2);
	assert(M1->n_rows == M2->n_rows);
	assert(M1->n_cols == M2->n_cols);

	for (i=0; i < M1->n_elements; ++i) {
		M1->data[i] = M1->data[i] + M2->data[i];
	}
}

/*  C = alpha*A*B + beta*C */
void matrix_times_matrix(Matrix* C, const Matrix* A, int A_trans, const Matrix* B, int B_trans, float alpha, float beta)
{
  unsigned int i, j, k, m, n, o;
  float temp_f;

  assert(A);
  assert(B);
  assert(C);
  assert(A_trans && B_trans ? A->n_rows==B->n_cols : 1); /* check INPUT dimensionality*/
  assert(A_trans && B_trans ? (C->n_rows==A->n_cols &&  C->n_cols==B->n_rows) : 1); /* check OUTPUT dimensionality*/

  assert(A_trans && !B_trans ? A->n_rows==B->n_rows : 1);
  assert(A_trans && !B_trans ? (C->n_rows==A->n_cols && C->n_cols==B->n_cols) : 1);

  assert(!A_trans && B_trans ? A->n_cols==B->n_cols : 1);
  assert(!A_trans && B_trans ? (C->n_rows==A->n_rows && C->n_cols==B->n_rows) : 1);

  assert(!A_trans && !B_trans ? A->n_cols==B->n_rows : 1);
  assert(!A_trans && !B_trans ? (C->n_rows==A->n_rows && C->n_cols==B->n_cols) : 1);

  m = A->n_rows;
  n = A->n_cols;
  o = B->n_cols;

  /* N.B.: remember to enable the SSE extensions at compile time */
  for (i=0; i < m; ++i) {
      for (k=0; k < o; ++k) {
          temp_f = 0;
          for (j=0; j < n; ++j) {
              temp_f = temp_f + matrix_get(A, i, j)*matrix_get(B, j, k);
          }
          matrix_set(C, i, k, alpha*temp_f + beta*matrix_get(C, i, k));
      }
  }
}

void matrix_set(Matrix* M, unsigned int i, unsigned int j, float value)
{
	assert(M);
	M->data[i*M->n_cols + j] = value;
}

float matrix_get(const Matrix* M, unsigned int i, unsigned int j)
{
	float res;

	assert(M);
	res = M->data[i*M->n_cols + j];
	return res;
}

int vector_cpy(Matrix* v1, const Matrix* v2, unsigned int start, unsigned int end)
{
	assert(v1);
	assert(v2);
	assert(v1->n_cols==1 || v1->n_rows==1);
	assert(v2->n_cols==1 || v2->n_rows==1);
	assert(end < v2->n_elements);
	assert((end-start) < v1->n_elements);

	memcpy(v1->data, v2->data + start, (end-start+1)*sizeof(float));

	return 0;
}

float matrix_sum_all(const Matrix* M)
{
	unsigned int i;
	float res;

	assert(M);

	res = 0;
	for (i=0; i < M->n_elements; ++i) {
	  res += M->data[i];
	}

	return res;
}

void matrix_normalize_p_norm(Matrix* M, float p)
{
	unsigned int i,j;
	double* sum_cols = NULL; /*n-dimensional array. each i-th entry is the sum of the i-th column*/

	assert(M);

	sum_cols = (double*) calloc(M->n_cols, sizeof(double));
	for (j=0; j < M->n_cols; ++j) {
		for (i=0; i < M->n_rows; ++i) {
			sum_cols[j] = sum_cols[j] + pow(fabs(M->data[i*M->n_cols + j]), p);
		}
	}

	for (j=0; j < M->n_cols; ++j) {
		sum_cols[j] = pow(sum_cols[j], 1.0/p);
		/* if sum_cols[j] is close to the machine-precision or it is zero, we set it to 1, in order to avoid numerical problems in the next for-loop */
		if (fabs(sum_cols[j]) < 2*DBL_MIN) {
			sum_cols[j] = 1.0;
		}
	}

	for (i=0; i < M->n_rows; ++i) {
		for (j=0; j < M->n_cols; ++j) {
			M->data[i*M->n_cols + j] = M->data[i*M->n_cols + j] / (float)sum_cols[j];
		}
	}

	free(sum_cols);
}


Matrix* matrix_clone(const Matrix* M2)
{
	Matrix* M1 = NULL;

	assert(M2);

	M1 = (Matrix*) malloc(sizeof(Matrix));
	assert(M1); /* TODO */
	create_matrix(M1, M2->n_rows, M2->n_cols);
	memcpy(M1->data, M2->data, M2->n_elements*sizeof(float));

	return M1;
}

void matrix_clone2(Matrix* M1, const Matrix* M2)
{
	assert(M1);
	assert(M2);

	create_matrix(M1, M2->n_rows, M2->n_cols);
	memcpy(M1->data, M2->data, M2->n_elements*sizeof(float));
}

int is_vector(const Matrix* M)
{
	int res;

	assert(M);

	if (M->n_cols==1 || M->n_rows==1) {
		res = 1;
	} else {
		res = 0;
	}

	return res;
}

Matrix* matrix_get_row(const Matrix* M, unsigned int k, int trans)
{
	 Matrix* v = NULL;

	assert(M);
	assert(k < M->n_rows);

	v = (Matrix*) malloc(sizeof(Matrix));
	if (!v) {
		exit(ERROR_MEM);
	}
	if (trans) {
		create_matrix(v, M->n_cols, 1);
	} else {
		create_matrix(v, 1, M->n_cols);
	}

	memcpy(v->data, M->data + k*M->n_cols, M->n_cols*sizeof(float));

	return v;
}


float relative_error_matrix_matrix(const Matrix* M1, const Matrix* M2)
{
	unsigned int i;
	double total_error;
	double A;
	double B;
	const double min_value = pow(10.0, -5.0);

	assert(M1->n_cols == M2->n_cols);
	assert(M1->n_rows == M2->n_rows);

	total_error = 0.0;
	for (i=0; i < M1->n_elements; ++i) {
		A = fabs(M1->data[i]-M2->data[i]);
		B = fabs(M1->data[i]);
		if (A > min_value && B > min_value) {
			total_error = total_error + A / B;
		} else if (A < min_value  && B < min_value) {
			total_error = total_error + 0.0;
		} else if (A < min_value) {
			total_error = total_error + 0.0;
		} else if (B < min_value) {
			//printf("M1:%f - M2:%f - A: %f - B:%f - B+min:%f - A/(B+min):%f\n", M1->data[i], M2->data[i], A, B, B+min_value, A / (B+min_value));
			total_error = total_error + A / (B+min_value);
		}
	}
	return (float)(total_error / M1->n_elements);
}


Matrix* concatenate_vectors(Matrix** M2, unsigned int num_matrix)
{
	unsigned int i;
	unsigned int n_rows;
	unsigned int n_cols;
	unsigned int offset;
	Matrix* M3 = NULL;

	assert(M2);
	assert(num_matrix > 0);
	/*check whether is or not a set of vectors*/
	if ((M2[1])->n_rows == 1) {
		for (i=0; i < num_matrix; ++i) {
			assert((M2[i])->n_rows == 1);
		}
	} else if ((M2[1])->n_cols == 1) {
		for (i=0; i < num_matrix; ++i) {
			assert((M2[i])->n_cols == 1);
		}
	} else {
		assert(0);
	}

	/* calculate the number of rows/cols */
	if ((M2[1])->n_rows == 1) {
		n_rows = 1;
		n_cols = 0;
		for (i=0; i < num_matrix; ++i) {
			n_cols = n_cols + (M2[i])->n_cols;
		}
	} else {
		n_rows = 0;
		n_cols = 1;
		for (i=0; i < num_matrix; ++i) {
			n_rows = n_rows + (M2[i])->n_rows;
		}
	}

	/* create the matrix */
	M3 = (Matrix*) malloc(sizeof(Matrix));
	if (!M3) {
		exit(ERROR_MEM);
	}
	create_matrix(M3, n_rows, n_cols);

	/* copy the data */
	offset = 0;
	for (i=0; i < num_matrix; ++i) {
		memcpy(offset + M3->data, (M2[i])->data, (M2[i])->n_elements*sizeof(float));
		offset = offset + (M2[i])->n_elements;
	}

	/* return */
	return M3;
}


/* direction: 0 vertical, 1 horizontal */
Matrix* concatenate_matrix(Matrix** M2, unsigned int num_matrix, unsigned int direction)
{
	unsigned int i, j;
	unsigned int n_rows;
	unsigned int n_cols;
	unsigned int offset;
	Matrix* M3 = NULL;

	assert(M2);
	assert(num_matrix > 0);
	assert((direction==0) || (direction==1));

	/*check whether is or not the dimensionality of each matrix is ok*/
	if (direction==0) {
		// vertical appending
		n_rows = 0;
		n_cols = (M2[0])->n_cols;
		for (i=0; i < num_matrix; ++i) {
			n_rows += (M2[i])->n_rows;
			assert((M2[i])->n_cols == (M2[0])->n_cols);
		}
	} else {
		// horizontal appending
		n_rows = (M2[0])->n_rows;
		n_cols = 0;
		for (i=0; i < num_matrix; ++i) {
			n_cols += (M2[i])->n_cols;
			assert((M2[i])->n_rows == (M2[0])->n_rows);
		}
	}

	/* create the matrix */
	M3 = (Matrix*) malloc(sizeof(Matrix));
	if (!M3) {
		exit(ERROR_MEM);
	}
	create_matrix(M3, n_rows, n_cols);

	/* copy the data */
	if (direction==0) {
		// vertical appending
		offset = 0;
		for (i=0; i < num_matrix; ++i) {
			memcpy(offset + M3->data, (M2[i])->data, (M2[i])->n_elements*sizeof(float));
			offset += (M2[i])->n_elements;
		}
	} else {
		// horizontal appending
		/* TODO remove the following assert and debug the code before using it (it should be fine however)*/
		assert(0);
		offset = 0;
		for (i=0; i < n_rows; ++i) {
			for (j=0; j < num_matrix; ++j) {
				/* we copy a line (row) at a time */
				memcpy(offset + M3->data, ((M2[i])->data)+i*((M2[i])->n_cols), (M2[i])->n_cols*sizeof(float));
				offset += (M2[i])->n_cols;
			}
		}
	}

	/* return */
	return M3;
}



float matrix_op_platt_support(float val, float sig_t, float sig_b) {
	float res;

	res = 1.0 / (1.0 + exp(-2.0/sig_t*val + sig_b));

	return res;
}

void matrix_op_platt(Matrix* M, const Matrix* Sig_T, const Matrix* Sig_B)
{
  unsigned int i, j;

  assert(M);
  assert(Sig_T);
  assert(Sig_B);
  assert(Sig_T->n_cols == Sig_B->n_cols);
  assert(Sig_T->n_rows == Sig_B->n_rows);

  if (Sig_T->n_cols==1 && Sig_T->n_rows==1) {
    /*scalar*/
    for (i=0; i < M->n_elements; ++i) {
      M->data[i] = matrix_op_platt_support(
        M->data[i], Sig_T->data[0], Sig_B->data[0]);
    }
  } else if (Sig_T->n_cols==1 && (Sig_T->n_rows==M->n_rows)) {
    /* vertical vector */
    for (i=0; i < M->n_rows; ++i) {
      for (j=0; j < M->n_cols; ++j) {
	matrix_set(M, i, j, matrix_op_platt_support(
	  matrix_get(M,i,j), Sig_T->data[i], Sig_B->data[i]));
      }
    }
  } else if (Sig_T->n_rows==1 && (Sig_T->n_cols==M->n_cols)) {
    /* horizontal vector */
    for (i=0; i < M->n_rows; ++i) {
      for (j=0; j < M->n_cols; ++j) {
	matrix_set(M, i, j, matrix_op_platt_support(
	  matrix_get(M,i,j), Sig_T->data[j], Sig_B->data[j]));
      }
    }
  } else if ((Sig_T->n_cols==M->n_cols) && (Sig_T->n_rows==M->n_rows)) {
    /* matrix */
    for (i=0; i < M->n_elements; ++i) {
      M->data[i] = matrix_op_platt_support(
        M->data[i], Sig_T->data[i], Sig_B->data[i]);
    }
  } else {  /*not supported */
    assert(1);
  }
}
