/***************************************************************************
 *            list_string.h
 *
 *  Mon Jul 26 15:09:13 2010
 *  Copyright  2010  Alessandro Bergamo
 *  <aleb@cs.dartmouth.edu>
 ****************************************************************************/

#ifndef LIST_STRING_H
#define LIST_STRING_H

#define INITIAL_LIST_STRING_CAPACITY 1000

#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

struct struct_list_string {
	char** data;
	unsigned int n_elements;
	unsigned int capacity;
};
typedef struct struct_list_string ListString;

/* create an empty list with an initial capacity of INITIAL_LIST_STRING_CAPACITY */
void create_empty_list_string(ListString* ls);

void delete_list_string(ListString* ls);

/* s will not be copied!!*/
void add_string_to_list(ListString* ls, char* s);

/* Double the capacity of the list string.auto
   N.B: private method!
*/
void double_list_string(ListString* ls);

char* get_string_from_list(const ListString* ls, unsigned int i);

/* ls must be an empty list */
int read_ascii_list_string(const char* file_name, ListString* ls);

int write_ascii_list_string(const char* file_name, const ListString* ls);

void print_list_string(const ListString* ls);

#ifdef __cplusplus
}
#endif

#endif


