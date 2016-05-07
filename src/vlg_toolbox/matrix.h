/***************************************************************************
 *            matrix.h
 *
 *  Mon Jul 26 15:09:13 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#if defined(__unix) || defined(__linux)
#include <cblas.h>
#endif

#define ERROR_IO -1
#define ERROR_MEM -2
#define ERROR_IO_MAT -3

#ifdef __cplusplus
extern "C" {
#endif

/*
OOP-style class.
All the methods (the functions which the first argument is Matrix* needs an objecte that has been already 
allocated in memory). Morever delete_matrix do not free the memory needs by the structure, it frees the memory 
of the internal structures.
*/
struct struct_matrix {
	unsigned int n_rows; /* public: ro*/
	unsigned int n_cols; /* public: ro*/
	unsigned int n_elements; /* public: ro  . It' just n_rows*n_cols */
	float* data;  /* private */
};
typedef struct struct_matrix Matrix;

/*
  Let's create a matrix with indefinites values
  ERRORS: [ERROR_MEM]
*/
int create_matrix(Matrix* M, unsigned int n_rows, unsigned int n_cols);

/* Let's create a matrix initialized by zero
  ERRORS: [ERROR_MEM]
*/
int create_matrix_zero(Matrix* M, unsigned int n_rows, unsigned int n_cols);

/* Let's create a matrix initialized by the array data. data must has n_rows*n_cols values
  N.B.: we do NOT perform a copy of the data vector we just assign the pointer!!!!
    ERRORS: [ERROR_MEM]
*/
int create_matrix_initialized(Matrix* M, float* data, unsigned int n_rows, unsigned int n_cols);

/* Let's create a matrix with random values in [0,1]
  ERRORS: [ERROR_MEM]
*/
int create_matrix_random(Matrix* M, unsigned int n_rows, unsigned int n_cols);

/* Delete all the internal structures of a object Matrix. N.B.: do not delete M!!! */
int delete_matrix(Matrix* M);

/* Read a matrix from a file in ascii-format
  ERRORS: ERROR_IO [ERROR_MEM]
 */
int read_ascii_matrix(Matrix* M, const char* file_name, unsigned int n_rows, unsigned int n_cols);

/*
 * Write a matrix to a file in ascii-format 
  ERROR: ERROR_IO
 */
int write_ascii_matrix(const Matrix* M, const char* file_name);

/* matrix's format: <n_rows: uint><n_cols:uint><data:float (row ordererd!)....... >
ERROR:  ERROR_IO
*/
int read_float_matrix(Matrix* M, const char* file_name);

/*
ERROR:  ERROR_IO
*/
int write_float_matrix(const Matrix* M, const char* file_name);

/* M must be a vector. M=M>0; It saves a binarized (to zero) version of the vector. Each bit is 1 entry.
The last bits of the last byte are set to zero 
ERROR:  ERROR_IO
*/
int write_bit_vector(const Matrix* v, const char* file_name);

/*
 * Write a matrix to the STOUT
 */
void print_ascii_matrix(const Matrix* M);

/* Let's apply sin() at each element of the matrix 
*/
void matrix_op_sin(Matrix* M);

void matrix_op_cos(Matrix* M);

/* natural log*/
void matrix_op_log(Matrix* M);

void matrix_op_sqrt(Matrix* M);

void matrix_op_divide(Matrix* M, float c);

/* M(.) = 1 if M(.) > thresh; 0 otherwise*/
void matrix_op_binarize(Matrix* M, float threshold);

void scalar_times_matrix(Matrix* M, float alpha);

/*
 M must be a vector
 same behavior as Matlab, Mout = repmat(M, [y, x]);
 */
Matrix* rep_vect(const Matrix* M, unsigned int y, unsigned int x);

/* M1 = M1.*M2 (matlab notation) */
void matrix_times_matrix_element( Matrix* M1, const Matrix* M2);

/* M1 = M1 + M2 */
void matrix_plus_matrix(Matrix* M1, const Matrix* M2);

/*  C = alpha*A*B + beta*C  */
void matrix_times_matrix(Matrix* C, const Matrix* A, int A_trans, const Matrix* B, int B_trans, float alpha, float beta);

void matrix_set(Matrix* M, unsigned int i, unsigned int j, float value);

float matrix_get(const Matrix* M, unsigned int i, unsigned int j);

/* v2 to v1*/
int vector_cpy(Matrix* v1, const Matrix* v2, unsigned int start, unsigned int end);

float matrix_sum_all(const Matrix* M);

/* Normalize the columns of M by the p-norm.  M = M ./ repmat((sum(abs(M).^p, 1)).^(1/p),[nDim,1]); */
void matrix_normalize_p_norm(Matrix* M, float p);


Matrix* matrix_clone(const Matrix* M2);

/* clone matrix M2 to M1. M1 must a valid pointer!*/
void matrix_clone2(Matrix* M1, const Matrix* M2);

/* return 1 if is a vector, 0 otherwise */
int is_vector(const Matrix* M);

/* if (trans) v=B(k,:)' else v=B(k,:) 
*/
Matrix* matrix_get_row(const Matrix* M, unsigned int k, int trans);

/*
 * return mean(mean(abs(M1-M2)./abs(M1)))
 * */
float relative_error_matrix_matrix(const Matrix* M1, const Matrix* M2);

Matrix* concatenate_vectors(Matrix** M2, unsigned int num_matrix);

/* direction: 0 vertical, 1 horizontal */
Matrix* concatenate_matrix(Matrix** M2, unsigned int num_matrix, unsigned int direction);

void matrix_op_platt(Matrix* M, const Matrix* Sig_T, const Matrix* Sig_B);



#ifdef __cplusplus
}
#endif

#endif  /* MATRIX_H */


