/***************************************************************************
 *            utils.h
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#define PI 3.14159265358979323846
#define MAX_CHARACTERS_LINE 5000

/* **bool** type    (implemented as int) */
#ifndef __cplusplus
#define false 0
#define true 1
typedef int bool;
#endif

/*
 * Macro to retrieve the target machine and set the system path separator.
 *
 * TODO These macro should be work with GCC and Visual-C. Other compilers???? cygwin??
*/
#if defined(__unix) || defined(__linux) || defined(__CYGWIN__) || defined(macintosh) || defined(__APPLE__)
#define SYSTEM_PATH_SEPARATOR '/'
#define SYSTEM_TYPE_NIX
#elif defined(_WIN32)
#define SYSTEM_PATH_SEPARATOR '\\'
#define SYSTEM_TYPE_WIN
#endif
#if defined(__unix) || defined(__linux)
#define SYSTEM_TYPE_LINUX
#endif
#if defined(macintosh) || defined(__APPLE__)
#define SYSTEM_TYPE_MAC
#endif

/*
 * GCC version string
 * */
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#define BUFFER_LENGTH 4096


#include <time.h>
typedef clock_t ClockTime; /* clock system-independent representation */

#ifdef __cplusplus
extern "C" {
#endif

/* get the time */
ClockTime tic();
/* get the time and return the time elapsed (calculated from the moment t) in seconds */
double toc(ClockTime t);

/*return 0 if successfull; otherwise non-zero */
int parse_float(const char* s, float* f);
int parse_int(const char* s, int* i);
int parse_uint(const char* s, unsigned int* ui);

/* returns a n-dimensional array, containing values [0..n-1]
 * perm must be preallocated */
void randperm(int n, int* perm);

/* It deletes the extension from the filename s.
 * E.g. "/tmp/filename.dat" ----->  "/tmp/filename" */
void string_delete_extension(char* s);

char* string_clone(const char* s);

/* It deletes the last path separator, if it is present and it is the last charachter
 * E.g.   "/tmp/dir1/"  ---> "/tmp/dir"
 * */
void string_delete_last_separator(char *s);

char* dirname(const char* s);

/* Given an input path, it creates all the missing directories recursively
 * we assume that path DOES NOT contain the file name!! */
int create_missing_dirs(const char* path);

/* it returns 1 if the file exists, 0 otherwise */
int exist_file(const char* file_name);

/* It runs the command <cmd> and returns the STDOUT.
 * NOTE: The lenght of the stdout string must be at most 8192 characters */
char* execute_external_command(const char* cmd);

/* total number of CPU-cores */
int get_num_cpu_cores();

/* get machine loading, using the utility "w" */
double get_machine_loading();


int save_float_matrix(float* data, unsigned int n_rows, unsigned int n_cols, const char* file_name);
int load_float_matrix(float** data, unsigned int* n_rows, unsigned int* n_cols, const char* file_name);




#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */


