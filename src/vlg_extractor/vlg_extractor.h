/*
 *  vlg_extractor.h
 *  Xcode_vlg_extractor
 *
 *  Created by Alessandro Bergamo on 2/14/12.
 *  Copyright 2012 Dartmouth College. All rights reserved.
 *
 */

#ifndef VLG_EXTRACTOR_H
#define VLG_EXTRACTOR_H

#include <vlg_toolbox/matrix.h>
#include <vlg_toolbox/list_string.h>
#include <vlg_toolbox/utils.h>

#define PROGRAM_NAME "vlg_extractor"
#define VERSION "1.1.3"
#define HELP_FILE "./HELP.txt"
#define DEFAULT_TEST_DIR "test"
#define DEFAULT_DATA_DIR "data"
#define DEFAULT_PREFIX_CONCATENATIONLOWLEVELFEATURES "_low_level_features_v2"
#define DEFAULT_PREFIX_CLASSEMES "_classemes"
#define DEFAULT_PREFIX_CLASSEMESBIN "_classemesBIN"
#define DEFAULT_PREFIX_PICODES128 "_picodes128"
#define DEFAULT_PREFIX_PICODES1024 "_picodes1024"
#define DEFAULT_PREFIX_PICODES2048 "_picodes2048"
#define DEFAULT_PREFIX_MC "_mc"
#define DEFAULT_PREFIX_MC_BIN "_mc_bit"

/* *************   class IO_Type   **************  */
#define IOTYPE_ASCII_EXTENSION ".ascii"
#define IOTYPE_FLOAT_VLG_EXTENSION ".dat"
#define IOTYPE_MAT_EXTENSION ".mat"
#define IOTYPE_BIN_EXTENSION ".bin"
#define IOTYPE_NDEF_TYPE 0
#define IOTYPE_ASCII_TYPE 1
#define IOTYPE_FLOAT_VLG_TYPE 2
#define IOTYPE_MAT_TYPE 4
#define IOTYPE_BIN_TYPE 8
#define IOTYPE_MAT_MAX_VARNAME_LENGTH 10
#define IOTYPE_MAX_PREFIX_LENGTH 128
#define DEFAULT_IOTYPE_MAT_VARNAME "p"
struct struct_IO_Type {
	unsigned int type;
	char MAT_varname[IOTYPE_MAT_MAX_VARNAME_LENGTH];
	char suffix[IOTYPE_MAX_PREFIX_LENGTH];
};
typedef struct struct_IO_Type IO_Type;

void IO_Type_constructor(IO_Type* obj, unsigned int _type, const char* _MAT_varname, const char* _suffix);

/* ************************************************************************ */




/* **************** class struct_Program_Options   ************************* */
struct struct_Program_Options {
	/* let's define the program options.
	 * They will be initialized has "not-set" (NULL pointers, INT/FLT_MAX, NOT_SET, None) or the defaults values.
	 * N.B.: root_path is NOT initialized (it is the only exception )*/
	char* root_path;
	char* test_path;
	char* input_filename;
	char* output_filename;
	char* list_input_filename;
	char* input_path;
	char* output_path;
	char* parameters_dir;
	bool output_overwriting;
	IO_Type load_concatenationLowLevelFeatures;
	/* actions */
	IO_Type save_classemes;
	IO_Type save_classemes_bin;
	IO_Type save_picodes128;
	IO_Type save_picodes1024;
	IO_Type save_picodes2048;
	IO_Type save_mc;
	IO_Type save_mc_bin;
	IO_Type save_concatenationLowLevelFeatures;
};
typedef struct struct_Program_Options Program_Options;

void Program_Options_constructor(Program_Options* obj, const char* root_dir);

void Program_Options_destructor(Program_Options* obj);

/* ************************************************************************ */



void command_help(Program_Options* options);

void command_test(Program_Options* options);

void command_extract_features(Program_Options* options);


/* if (value == NULL) { printf("ERROR we need a value the this option ") } */
void check_option_value(const char* option, const char* value);

/*
 * In the case there is the possibility to enter 2 input file options such:
 * - unique file name
 * - ASCII file in which each row is the filename of an image/file
 * this function converto the user input argoment to a couple of ListString
 *
 * _ls_input/_ls_output. To note that we will need to delete manually this structures when we will no longer need
 * _create_missing_output_directories: if true create any missing directories in all the output filenames
 * _suffix. If present, for each output filename we delete the extension, then we append the suffix. Otherwise the output filenanes are just without the extension
 * */
void input_filenames_2_liststring(ListString** _ls_input, ListString** _ls_output, Program_Options* options, int _create_missing_output_directories, const char* _suffix);

/*
 * Given the file output path (*without* the extension!), it writes the file as specified in output_type.
 * If output_type is a MAT file, we also need to specify output_varname (the variable name).
 * output_type can contain multiple values; in this case the function will write the data on different files.
 * If output_overwriting, we overwrite the output file, if it exist. Otherwise we simply skip the writing.
 *  */
int write_output(const char* file_name, const Matrix* M, const IO_Type* output_type, bool output_overwriting);

/* Given the filename (*without* the extension!), it reads the data from the disk, filling the Matrix M.
 * The function will invoke create_matrix(M, ...).
 * You have to specify the input type by setting input_type that can be MAT,ASCII,FLOAT (only one!)
 * */
int read_input(const char* file_name, Matrix* M, const IO_Type* input_type);

/*
 * Given the file_name *without* the extension, it will check if the output file exist.
 * We return 1 if the output exists, 0 otherwise
 * The output type must to be specified by output_type. If output_type contain more formats, we will return 1 iff all the outputs exist.
 * */
int exist_output(const char* file_name, const IO_Type* output_type);




#endif /* VLG_EXTRACTOR_H */

