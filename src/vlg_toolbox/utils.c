/***************************************************************************
 *            utils.c
 *
 *  Thu Jul 22 16:07:01 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#if defined(SYSTEM_TYPE_NIX)
#include <unistd.h>  /* we include this file for the access() */
#include <sys/stat.h>
#elif defined(SYSTEM_TYPE_WIN)
#include <Windows.h> /* we include this file for the GetFileAttributes() */
#endif

ClockTime tic()
{
  return clock();
}

double toc(ClockTime t)
{
  clock_t t_now;
  
  t_now = clock();
  return (double)(t_now - t)/(double)CLOCKS_PER_SEC;
}

int parse_float(const char* s, float* f)
{
	float temp_f;
	char* pos;
	
	assert(s);
	assert(f);

	temp_f = (float) strtod(s, &pos);
	if (*pos == '\x0') {
		/* the conversion was successfully*/
		*f = temp_f;
		return 0;
	} else {
		return 1;
	}
}

int parse_int(const char* s, int* i)
{
	char* pos;
	
	assert(s);
	assert(i);

	*i = (int) strtol(s, &pos, 0);
	if (*pos == '\x0') {
		/* the conversion was successfully*/
		return 0;
	} else {
		return 1;
	}
}

int parse_uint(const char* s, unsigned int* ui)
{
	char* pos;
	
	assert(s);
	assert(ui);

	*ui = (unsigned int) strtoul(s, &pos, 0);
	if (*pos == '\x0') {
		/* the conversion was successfully*/
		return 0;
	} else {
		return 1;
	}
}


void randperm(int n, int* perm)
{
	int i, j, t;

	for(i=0; i<n; i++)
		perm[i] = i;
	for(i=0; i<n; i++) {
		j = rand()%(n-i)+i;
		t = perm[j];
		perm[j] = perm[i];
		perm[i] = t;
	}
}


void string_delete_extension(char* s)
{
	assert(s);

	/* we have to find the last '.' */
	 s = strrchr(s, '.');
	 if (s) {
		*s = '\0';
	 }
}

char* string_clone(const char* s)
{
	char* s_out = NULL;
	
	assert(s);

	s_out = (char*) malloc(strlen(s)+1);
	assert(s_out);
	strcpy(s_out, s);	
	return s_out;
}

void string_delete_last_separator(char *s) 
{
	size_t n;
	
	assert(s);

	n = strlen(s);
	if (s[n-1] == SYSTEM_PATH_SEPARATOR) {
		s[n-1] = '\0';
	}
}

char* dirname(const char* s)
{
	char* s_out = NULL;
	char* pos = NULL;
	size_t len;

	assert(s);

	len = strlen(s)+1;
	s_out = (char*) malloc(len*sizeof(char));
	strcpy(s_out, s);
	string_delete_last_separator(s_out);
	pos = strrchr(s_out, SYSTEM_PATH_SEPARATOR);
	if (pos == NULL) {
		*s_out = '\0';
	} else {
		*pos = '\0';
	}

	return s_out;
}

int create_missing_dirs(const char* path)
{
	assert(path);

#if defined(SYSTEM_TYPE_NIX)
	char buffer[MAX_CHARACTERS_LINE];
    char *p = NULL;
    size_t len;

	printf("creating directory %s\n", path);

	snprintf(buffer, sizeof(buffer), "%s", path);
	len = strlen(buffer);
	if (buffer[len-1] == '/') {
		buffer[len-1] = 0;
	}
	for (p = buffer + 1; *p; p++) {
		if (*p == '/') {
			*p = 0;
			mkdir(buffer, 0755);
			*p = '/';
		}
	}
	mkdir(buffer, 0755);

	/* system() version */
	/*sprintf(buffer, "mkdir -p %s", path);
	system(buffer);*/
#elif defined(SYSTEM_TYPE_WIN)
	char buffer[MAX_CHARACTERS_LINE];

	printf("creating directory %s\n", path);

	sprintf(buffer, "mkdir %s", path);
	system(buffer);
#else
#error create_missing_dirs
#endif

	return 0; /*TODO*/
}


int exist_file(const char* file_name)
{
	assert(file_name);

#if defined(SYSTEM_TYPE_NIX)
	if (!access(file_name, F_OK)) {
		return 1;
	} else {
		return 0;
	}
#elif defined(SYSTEM_TYPE_WIN)
    if (0xFFFFFFFF == GetFileAttributes(file_name)) {
    	return 0;
    } else {
    	return 1;
    }
#endif
}


char* execute_external_command(const char* cmd) {
#if defined(SYSTEM_TYPE_LINUX)
	  FILE *in;
	  char buff[8192];
	  char* output = (char*) malloc(8192*sizeof(char));
	  int offset = 0;

	  /* popen creates a pipe so we can read the output
	     of the program we are invoking */
	  if (!(in = popen(cmd, "r"))) {
	    assert(0);
	  }

	  /* read the output of netstat, one line at a time */
	  offset = 0;
	  while (fgets(buff, sizeof(buff), in) != NULL ) {
	    //printf("Output: %s", buff); // DEBUG
	    strcpy(output+offset, buff);
	    offset = offset + strlen(buff);
	  }

	  /* close the pipe */
	  pclose(in);

	  return output;
#else
	return NULL;
#endif
}


int get_num_cpu_cores() {
	char* output_cmd = NULL;
	int n;
#if defined(SYSTEM_TYPE_LINUX)
	 output_cmd = execute_external_command("cat /proc/cpuinfo | grep processor | wc -l");
	 n = atoi(output_cmd);
	 free(output_cmd);
	 return n;
#elif defined(SYSTEM_TYPE_MAC)
	 output_cmd = execute_external_command("sysctl -n hw.ncpu");
	 n = atoi(output_cmd);
	 free(output_cmd);
	 return n;
#else
	/* unknown system-type */
	return 1;
#endif
}


double get_machine_loading() {
	int num_cpu_cores;
	double kernel_loading;
	char* temp_s = NULL;

#if defined(SYSTEM_TYPE_LINUX)
	num_cpu_cores = get_num_cpu_cores();
	temp_s = execute_external_command("w | head -n 1 | awk '{print $10}'");
	kernel_loading = (double) atof(temp_s);
	free(temp_s);
	return kernel_loading / (double)num_cpu_cores;
#elif defined(SYSTEM_TYPE_MAC)
	num_cpu_cores = get_num_cpu_cores();
	temp_s = execute_external_command("w | head -n 1 | awk '{print $10}'");
	kernel_loading = (double) atof(temp_s);
	free(temp_s);
	return kernel_loading / (double)num_cpu_cores;
#else
	/* unknown system-type. The machine looks always busy */
	return 1.0;
#endif
}




int save_float_matrix(float* data, unsigned int n_rows, unsigned int n_cols, const char* file_name)
{
	FILE* fout = NULL;
	size_t res;
	int res2;
	unsigned long int n_elements;

	assert(file_name);
	assert(data);

	/* open the file */
	fout = fopen(file_name, "wb");
	if (!fout) {
		return -1;
	}
	/* write n_rows,n_cols,data */
	res = fwrite(&n_rows, sizeof(unsigned int), 1, fout);
	assert(res > 0);
	res = fwrite(&n_cols, sizeof(unsigned int), 1, fout);
	assert(res > 0);
	n_elements = (unsigned long int)n_rows*n_cols;
	res = fwrite(data, sizeof(float), n_elements, fout);
	assert(res == n_elements);
	/* close the file and return */
	res2 = fclose(fout);
	assert(!res2);

	return 0;
}

int load_float_matrix(float** data, unsigned int* n_rows, unsigned int* n_cols, const char* file_name)
{
	FILE* fin = NULL;
	unsigned int temp_uint;
	size_t res;
	int res2;
	unsigned long int n_elements;

	assert(file_name);

	/* open the file */
	fin = fopen(file_name, "rb");
	if (!fin) {
		return -1;
	}
	/* read n_rows,n_cols*/
	res = fread(&temp_uint, sizeof(unsigned int), 1, fin);
	assert(res > 0);
	*n_rows = temp_uint;
	res = fread(&temp_uint, sizeof(unsigned int), 1, fin);
	assert(res > 0);
	*n_cols = temp_uint;
	/* create the Matrix */
	n_elements = (unsigned long int)(*n_rows) * (*n_cols);
	*data = (float*) malloc(n_elements*sizeof(float));
	if (!(*data)) {
		fclose(fin);
		return -2;
	}
	/* copy the data */
	res = fread(*data, sizeof(float), n_elements, fin);
	if(res != n_elements) {
		return -2;
	}
	/* close the file and exit */
	res2 = fclose(fin);
	assert(!res2);

	return 0;
}

