/***************************************************************************
 *            list_string.c
 *
 *  Mon Jul 26 15:09:13 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#include "list_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "utils.h"

void create_empty_list_string(ListString* ls)
{
#ifdef DEBUG0
	assert(ls);
#endif

	ls->data = (char**) malloc(INITIAL_LIST_STRING_CAPACITY*sizeof(char*));
	assert(ls->data);
	
	ls->capacity = INITIAL_LIST_STRING_CAPACITY;
	ls->n_elements = 0;
}

void delete_list_string(ListString* ls)
{
	unsigned int i;
	
#ifdef DEBUG0
	assert(ls);
#endif

	for (i=0; i < ls->n_elements; ++i) {
		free(ls->data[i]);
	}
	free(ls->data);
}

void add_string_to_list(ListString* ls, char* s) 
{
#ifdef DEBUG0
	assert(ls);
#endif

	if (ls->n_elements == ls->capacity) {
		double_list_string(ls);
	}
	
	ls->data[ls->n_elements] = s;
	ls->n_elements = ls->n_elements + 1;
}

void double_list_string(ListString* ls)
{
#ifdef DEBUG0
	assert(ls);
#endif

	ls->data = (char**) realloc(ls->data, 2*ls->capacity*sizeof(char*));
	assert(ls->data);
	ls->capacity = 2*ls->capacity;
}

char* get_string_from_list(const ListString* ls, unsigned int i)
{
#ifdef DEBUG0
	assert(ls);
	assert(i < ls->n_elements);
#endif
	
	return ls->data[i];
}

int read_ascii_list_string(const char* file_name, ListString* ls)
{
	FILE* fin;
	char* s;
	unsigned int len_s;
	char buffer[MAX_CHARACTERS_LINE];
	char* s2 = NULL;
	int res = -1;
	
#ifdef DEBUG0
	assert(file_name);
	assert(ls);
#endif

	fin = fopen(file_name, "r");
	if (!fin) {
		fprintf(stderr, "I/O ERROR %s\n", file_name);
		return -1;
	}
	
	create_empty_list_string(ls);
	
	s2 = fgets(buffer, MAX_CHARACTERS_LINE, fin);
	while ( s2 ) {
		s2 = strchr(buffer, '\n');
		if (s2) {
			*s2 = '\0';
		}
		len_s = strlen(buffer);
		if (len_s > 0) {
			s = (char*) malloc((len_s+1)*sizeof(char));
			strcpy(s, buffer);
			add_string_to_list(ls, s);
		}
		s2 = fgets(buffer, MAX_CHARACTERS_LINE, fin);
	}
	
	res = fclose(fin);
	if (res) {
		return -2;
	}

	return 0;
}

int write_ascii_list_string(const char* file_name, const ListString* ls)
{
	FILE* fout;
	unsigned int i;
	int res;
	
#ifdef DEBUG0
	assert(file_name);
	assert(ls);
#endif

	fout = fopen(file_name, "w");
	if (!fout) {
		fprintf(stderr, "I/O ERROR %s\n", file_name);
		return -1;
	}
	
	for (i=0; i < ls->n_elements; ++i) {
		res = fputs(ls->data[i], fout);
		assert(res);
		res = fputc('\n', fout);
		assert(res);
	}
	
	res = fclose(fout);
	if (res) {
		return -2;
	}

	return 0;
}

void print_list_string(const ListString* ls)
{
	unsigned int i;
	
#ifdef DEBUG0
	assert(ls);
#endif

	for (i=0; i < ls->n_elements; ++i) {
		printf("%s\n", ls->data[i]);
	}
}


