/*
 *  vlg_extractor.c
 *  Xcode_vlg_extractor
 *
 *  Created by Alessandro Bergamo on 2/14/12.
 *  Copyright 2012 Dartmouth College. All rights reserved.
 *
 */

#include "vlg_extractor.h"
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <vlg_toolbox/utils.h>
#include <vlg_toolbox/matrix.h>
#include <vlg_toolbox/matlab.h>
#include <vlg_toolbox/list_string.h>
#include "img_features.h"
#include "error.h"
#include "extract_low_level_features.h"
#include "extract_classemes.h"
#include "extract_picodes.h"
#include "extract_metaclass.h"
#include "vlg_extractor_help.h"


void exit_printing_error_code(int err) {
	if (!err) {
		return;
	}

	printf("ERROR CODE %d\n", err);
	exit(err);
}


void IO_Type_constructor(IO_Type* obj, unsigned int _type, const char* _MAT_varname, const char* _suffix)
{
	assert(_MAT_varname);
	assert(_suffix);
	assert(strlen(_MAT_varname) < IOTYPE_MAT_MAX_VARNAME_LENGTH);
	assert(strlen(_suffix) < IOTYPE_MAX_PREFIX_LENGTH);
	
	obj->type = _type;
	strcpy(obj->MAT_varname, _MAT_varname);
	strcpy(obj->suffix, _suffix);
}



unsigned int option_to_IOType(const char* s)
{
	if (!strcmp(s, "ASCII")) {
		return IOTYPE_ASCII_TYPE;
	}
	if (!strcmp(s, "FLOAT")) {
		return IOTYPE_FLOAT_VLG_TYPE;
	}
	if (!strcmp(s, "MAT")) {
#ifdef MAT_SUPPORT
		return IOTYPE_MAT_TYPE;
#else
		fprintf(stderr, ERROR_NOT_MAT_SUPPORT_STR"\nWriting a .mat file requires the Matlab support that has not been enabled at compile time.\n");
		exit(ERROR_NOT_MAT_SUPPORT);
#endif
	}
	if (!strcmp(s, "BIN")) {
		return IOTYPE_BIN_TYPE;
	}
	/* If here there is a parsing problem */
	printf(ERROR_PARSING_ARGUMENTS_STR": %s is not a file-type.\n", s);
	exit(ERROR_PARSING_ARGUMENTS);
	return ERROR_PARSING_ARGUMENTS;
}



void parse_options(Program_Options* options, int argc, char *argv[])
{
	int i;
	char* option = NULL;
	char* value = NULL;
	char buffer[MAX_CHARACTERS_LINE];

	/* For each option:
	 * -value is the
	 *  */
	for (i=1; i < argc-3; ++i) {
		/* if present, let's seek the character '=' and split the string */
		option = argv[i];
		value = strchr(argv[i], '=');
		if (value) {
			*value = '\0';
			++value;
		}

		if (!strcmp(option, "--help")) {
			command_help(options);
			exit(0);
		} else if (!strcmp(option, "--test")) { /* NOTE: this option is not documented */
			command_test(options);
			exit(0);
		}else if (!strcmp(option, "--input-filename")) { /* NOTE: this option is not documented */
			check_option_value(option, value);
			options->input_filename = string_clone(value);
		} else if (!strcmp(option, "--output-filename")) { /* NOTE: this option is not documented */
			check_option_value(option, value);
			options->output_filename = string_clone(value);
		} else if (!strcmp(option, "--parameters-dir")) {
			check_option_value(option, value);
			string_delete_last_separator(value);
			if (options->parameters_dir)
				free(options->parameters_dir);
			options->parameters_dir = string_clone(value);
		} else if (!strcmp(option, "--disable-output-overwriting")) {
			options->output_overwriting = 0;
		} else if (!strcmp(option, "--extract_classemes")) {
			check_option_value(option, value);
			options->save_classemes.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--extract_classemes_bin")) {
			check_option_value(option, value);
			options->save_classemes_bin.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--extract_picodes128")) {
			check_option_value(option, value);
			options->save_picodes128.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--extract_picodes1024")) {
			check_option_value(option, value);
			options->save_picodes1024.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--extract_picodes2048")) {
			check_option_value(option, value);
			options->save_picodes2048.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--extract_mc")) {
			check_option_value(option, value);
			options->save_mc.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--extract_mc_bit")) {
			check_option_value(option, value);
			options->save_mc_bin.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--classeme-suffix")) {
			check_option_value(option, value);
			strcpy(options->save_classemes.suffix, value);
			sprintf(buffer, "%sBIN", value);
			strcpy(options->save_classemes_bin.suffix, buffer);
		} else if (!strcmp(option, "--picodes-suffix")) {
			check_option_value(option, value);
			sprintf(buffer, "%s128", value);
			strcpy(options->save_picodes128.suffix, buffer);
			sprintf(buffer, "%s1024", value);
			strcpy(options->save_picodes1024.suffix, buffer);
			sprintf(buffer, "%s2048", value);
			strcpy(options->save_picodes2048.suffix, buffer);
		} else if (!strcmp(option, "--mc-suffix")) {
			check_option_value(option, value);
			strcpy(options->save_mc.suffix, value);
			sprintf(buffer, "%sBIN", value);
			strcpy(options->save_mc_bin.suffix, buffer);
#ifdef INCLUDE_EXTRA_FEATURES
		} else if (!strcmp(option, "--extract_concatenationLowLevelFeatures")) {
			check_option_value(option, value);
			options->save_concatenationLowLevelFeatures.type |= option_to_IOType(value);
		} else if (!strcmp(option, "--load-concatenation-low-level-features")) {
			check_option_value(option, value);
			options->load_concatenationLowLevelFeatures.type = option_to_IOType(value);
			/* NOTE. Loading of ASCII and BIN is not supported */
			if (options->load_concatenationLowLevelFeatures.type & IOTYPE_ASCII_TYPE) {
				printf("Loading of ASCII format is not currently supported.\n");
				exit(ERROR_NOT_YET_IMPLEMENTED);
			}
			if (options->load_concatenationLowLevelFeatures.type & IOTYPE_BIN_TYPE) {
				printf("Loading of BIN format is not currently supported.\n");
				exit(ERROR_NOT_YET_IMPLEMENTED);
			}
#endif
		} else {
			fprintf(stderr, ERROR_PARSING_ARGUMENTS_STR": option not recognized: %s\n", option);
			exit(ERROR_PARSING_ARGUMENTS);
		}
	}
}




int main(int argc, char *argv[])
{	
	Program_Options* options = NULL;
	char* root_dir = NULL;
	char* buffer = NULL;
	unsigned int mode = 0;

	/* the program is starting.. */
	printf("%s - version %s\n", PROGRAM_NAME, VERSION);

    
#if defined(SYSTEM_TYPE_WIN)
	/* Linux uses by default the extended-double FPU precision.
	 * We force it on Windows.
	 * NOTE: Linux on x86_64 uses by default the SSE op-codes, so I also switched the SSE extensions Pentium4 requirements (see Makefile_win) in order to try to generate the same code */
#if defined(__MINGW32__)
	mode = 0x037F;
	asm("fldcw %0" : : "m" (*&mode));
//#elif defined(_MSC_VER)
//	mode = _controlfp(_PC_53, _MCW_PC);
//	printf("mode=%x\n", mode);
//#else
//	assert(0);
#endif	
#endif


	/* create the Program_Options structure and initialize it */
#if defined(SYSTEM_TYPE_WIN)
	buffer = (char*) malloc(MAX_PATH*sizeof(char));
	GetModuleFileName(NULL, buffer, MAX_PATH);
	root_dir = dirname(buffer);
	free(buffer);
	buffer = NULL;
#else
	root_dir = dirname(argv[0]);
#endif
	options = (Program_Options*) malloc(sizeof(Program_Options));
	Program_Options_constructor(options, root_dir);

	if ((argc==1) || ((argc==2) && (!strcmp(argv[1], "--help")))) {
		command_help(options);
		exit(0);
	}
	if (argc < 4) {
		printf("ERROR: The program requires at least 3 parameters. See the file HELP.txt\n");
		exit(ERROR_GENERIC);
	}

	/* parsing the mandatory options */
	if (argc > 3) {
		options->list_input_filename  = string_clone(argv[argc-3]);
		options->input_path = string_clone(argv[argc-2]);
		options->output_path = string_clone(argv[argc-1]);
	}

	/* parsing the options */
	parse_options(options, argc, argv);
	
	/* run the feature extraction */
	command_extract_features(options);

	/* free */
	Program_Options_destructor(options);
	free(options);
	
    printf("EXIT\n");
    return (0);
}




void command_help(Program_Options* options)
{
    int i = 0;
    for (i = 0; i < HELP_txt_len; ++i) {
        printf("%c", HELP_txt[i]);
    }
}

								
								
								

void command_extract_features(Program_Options* options)
{
	ListString* list_img_input = NULL;
	ListString* list_img_output = NULL;
	int res = INT_MAX;
	unsigned int idx_img;
	char* img_in_fileName = NULL;
	char* img_out_fileName = NULL;
	char buffer[MAX_CHARACTERS_LINE];
	Matrix* feat = NULL;
	ImageData* img_data = NULL;
	Classemes* img_classemes = NULL;
	Picodes* img_picodes = NULL;
	Metaclass* img_mc = NULL;
	
	assert(options);
	
	input_filenames_2_liststring(&list_img_input, &list_img_output, options, 1, NULL);
	
	/* init */
	printf("Loading parameters... ");
	fflush(stdout);
	res = ImageData_init(options);
	if (res) {
		printf("Error during the global parameters loading. (code=%d)\n", res);
		exit(res);
	}
	if ((options->save_classemes.type != IOTYPE_NDEF_TYPE) || (options->save_classemes_bin.type != IOTYPE_NDEF_TYPE)) {
		res = Classemes_init(options);
		if (res) {
			printf("Error during the Classemes parameters loading. (code=%d)\n", res);
			exit(res);
		}
	}
	if ((options->save_picodes128.type != IOTYPE_NDEF_TYPE) || (options->save_picodes1024.type != IOTYPE_NDEF_TYPE) || (options->save_picodes2048.type != IOTYPE_NDEF_TYPE)) {
		res = Picodes_init(options);
		if (res) {
			printf("Error during the Picodes parameters loading. (code=%d)\n", res);
			exit(res);
		}
	}
	if ((options->save_mc.type != IOTYPE_NDEF_TYPE) || (options->save_mc_bin.type != IOTYPE_NDEF_TYPE)) {
		res = Metaclass_init(options);
		if (res) {
			printf("Error during the Metaclass parameters loading. (code=%d)\n", res);
			exit(res);
		}
	}
	printf("done.\n");
	fflush(stdout);

	/* for each image... */
	for (idx_img=0; idx_img < list_img_input->n_elements; ++idx_img) {
		img_in_fileName = get_string_from_list(list_img_input, idx_img);
		img_out_fileName = get_string_from_list(list_img_output, idx_img);

		printf("Processing image %s... ", img_in_fileName);
		fflush(stdout);

		/* construct image_data */
		img_data = (ImageData*) malloc(sizeof(ImageData));
		res =  ImageData_constructor(img_data, img_in_fileName, img_out_fileName);
		assert(!res);

		/* --extract_concatenationLowLevelFeatures */
		if (options->save_concatenationLowLevelFeatures.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_concatenationLowLevelFeatures.suffix);
			res = exist_output(buffer, &options->save_concatenationLowLevelFeatures);
			if (!res || options->output_overwriting) {
				res = ImageData_get_low_level_feature_concatenation(img_data, &feat);
				if (res) {
					printf("Error during feature extraction. (code=%d)\n", res);
					/* continue: all the next steps are based on this vector (currently) */
					ImageData_deconstructor(img_data);
					free(img_data);
					continue;
				}
				res = write_output(buffer, feat, &options->save_concatenationLowLevelFeatures, options->output_overwriting);
				exit_printing_error_code(res);
			} else {
				printf("Skip concatenationLowLevelFeatures. ");
			}
		}

		/* --extract_classemes */
		img_classemes = (Classemes*) malloc(sizeof(Classemes));
		Classemes_constructor(img_classemes, img_data);
		if (options->save_classemes.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_classemes.suffix);
			res = exist_output(buffer, &options->save_classemes);
			if (!res || options->output_overwriting) {
				res = Classemes_get_descriptor(img_classemes, &feat);
				if (res) {
					printf("Error during classemes extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_classemes, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip classemes. ");
			}
		}
		if (options->save_classemes_bin.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_classemes_bin.suffix);
			res = exist_output(buffer, &options->save_classemes_bin);
			if (!res || options->output_overwriting) {
				res = Classemes_get_descriptorBIN(img_classemes, &feat);
				if (res) {
					printf("Error during classemes_bin extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_classemes_bin, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip classemes_bin. ");
			}
		}
		Classemes_deconstructor(img_classemes);
		free(img_classemes);



		/* --extract_picodes128/1024/2048 */
		img_picodes = (Picodes*) malloc(sizeof(Picodes));
		Picodes_constructor(img_picodes, img_data);
		if (options->save_picodes128.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_picodes128.suffix);
			res = exist_output(buffer, &options->save_picodes128);
			if (!res || options->output_overwriting) {
				res = Picodes_get_descriptor128(img_picodes, &feat);
				if (res) {
					printf("Error during Picodes128 extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_picodes128, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip Picodes128. ");
			}
		}
		if (options->save_picodes1024.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_picodes1024.suffix);
			res = exist_output(buffer, &options->save_picodes1024);
			if (!res || options->output_overwriting) {
				res = Picodes_get_descriptor1024(img_picodes, &feat);
				if (res) {
					printf("Error during Picodes1024 extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_picodes1024, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip Picodes1024. ");
			}
		}
		if (options->save_picodes2048.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_picodes2048.suffix);
			res = exist_output(buffer, &options->save_picodes2048);
			if (!res || options->output_overwriting) {
				res = Picodes_get_descriptor2048(img_picodes, &feat);
				if (res) {
					printf("Error during Picodes2048 extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_picodes2048, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip Picodes2048. ");
			}
		}
		Picodes_deconstructor(img_picodes);
		free(img_picodes);


		/* --extract_mc */
		img_mc = (Metaclass*) malloc(sizeof(Metaclass));
		Metaclass_constructor(img_mc, img_data);
		if (options->save_mc.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_mc.suffix);
			res = exist_output(buffer, &options->save_mc);
			if (!res || options->output_overwriting) {
				res = Metaclass_get_descriptorPLATT(img_mc, &feat);
				if (res) {
					printf("Error during Metaclass extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_mc, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip Metaclass. ");
			}
		}
		if (options->save_mc_bin.type != IOTYPE_NDEF_TYPE) {
			sprintf(buffer, "%s%s", img_out_fileName, options->save_mc_bin.suffix);
			res = exist_output(buffer, &options->save_mc_bin);
			if (!res || options->output_overwriting) {
				res = Metaclass_get_descriptorBIN(img_mc, &feat);
				if (res) {
					printf("Error during Metaclass_bin extraction. (code=%d)\n", res);
				} else {
					res = write_output(buffer, feat, &options->save_mc_bin, options->output_overwriting);
					exit_printing_error_code(res);
				}
			} else {
				printf("Skip Metaclass_bin. ");
			}
		}
		Metaclass_deconstructor(img_mc);
		free(img_mc);


		/* free */
		ImageData_deconstructor(img_data);
		free(img_data);

		printf("done.\n");
		fflush(stdout);
	}
	
	/* end */
	if ((options->save_mc.type != IOTYPE_NDEF_TYPE) || (options->save_mc_bin.type != IOTYPE_NDEF_TYPE)) {
		Metaclass_end();
	}
	if ((options->save_picodes128.type != IOTYPE_NDEF_TYPE) || (options->save_picodes1024.type != IOTYPE_NDEF_TYPE) || (options->save_picodes2048.type != IOTYPE_NDEF_TYPE)) {
		Picodes_end();
	}
	if ((options->save_classemes.type != IOTYPE_NDEF_TYPE) || (options->save_classemes_bin.type != IOTYPE_NDEF_TYPE)) {
		Classemes_end();
	}
	ImageData_end();

	/* delete */
	delete_list_string(list_img_input);
	free(list_img_input);
	delete_list_string(list_img_output);
	free(list_img_output);
}



void command_test(Program_Options* options)
{
	//run_all_test(options);
}




void check_option_value(const char* option, const char* value)
{
	assert(option);
	
	if (!value) {
		fprintf(stderr, ERROR_PARSING_ARGUMENTS_STR": It's not been specified a value for the the option %s \n", option);
		exit(ERROR_PARSING_ARGUMENTS);
	}
}



void input_filenames_2_liststring(ListString** _ls_input, ListString** _ls_output, Program_Options* options, int _create_missing_output_directories, const char* _suffix)
{
	ListString* ls_input = NULL;
	ListString* ls_temp = NULL;
	ListString* ls_output = NULL;
	int res;
	char buffer[MAX_CHARACTERS_LINE];
	char buffer2[MAX_CHARACTERS_LINE];
	char* s = NULL;
	unsigned int i;
	
	assert(_ls_input);
	assert(_ls_output);
	assert(options);
	
	/* let's check the required parameters */
	if (!(options->input_filename || options->list_input_filename)) {
		fprintf(stderr, ERROR_MISSING_ARGUMENTS_STR": you have to specify --input-filename XOR --list-input-filename\n");
		exit(ERROR_MISSING_ARGUMENTS);
	}
	
	ls_input = (ListString*) malloc(sizeof(ListString));
	ls_output = (ListString*) malloc(sizeof(ListString));
	
	if (options->input_filename) {
		/* it's set a single file name */
		create_empty_list_string(ls_input);
		create_empty_list_string(ls_output);
		
		s = (char*) malloc((strlen(options->input_filename)+1)*sizeof(char));
		strcpy(s, options->input_filename);
		add_string_to_list(ls_input, s);
		
		if (!options->output_filename) {
			options->output_filename = options->input_filename;
		}
		strcpy(buffer, options->output_filename);
		if (_suffix) {
			strcat(buffer, _suffix);
		}
		s = (char*) malloc((strlen(buffer)+1)*sizeof(char));
		strcpy(s, buffer);
		add_string_to_list(ls_output, s);
	} else if (options->list_input_filename) {
		/* it's set a file containing a list of images */
		create_empty_list_string(ls_input);
		create_empty_list_string(ls_output);
		
		ls_temp = (ListString*) malloc(sizeof(ListString));
		res = read_ascii_list_string(options->list_input_filename, ls_temp);
		if (res) { exit(res); }
		for (i=0; i < ls_temp->n_elements; ++i) {
			sprintf(buffer, "%s%c%s", options->input_path, SYSTEM_PATH_SEPARATOR, get_string_from_list(ls_temp, i));
			s = (char*) malloc((strlen(buffer)+1)*sizeof(char));
			strcpy(s, buffer);
			add_string_to_list(ls_input, s);
			
			sprintf(buffer, "%s%c%s", options->output_path, SYSTEM_PATH_SEPARATOR, get_string_from_list(ls_temp, i));
			string_delete_extension(buffer);
			if (_suffix) {
				strcat(buffer, _suffix);
			}
			s = (char*) malloc((strlen(buffer)+1)*sizeof(char));
			strcpy(s, buffer);
			add_string_to_list(ls_output, s);
		}
		delete_list_string(ls_temp);
		free(ls_temp);
	}
	
	/* creating missing output directories */
	if (_create_missing_output_directories) {
		buffer2[0] = '\0'; /*zero-length string*/
		for (i=0; i < ls_output->n_elements; ++i) {
			/* remove the file name */
			strcpy(buffer, get_string_from_list(ls_output, i));
			s = strrchr(buffer, SYSTEM_PATH_SEPARATOR);
			if (s) {
				*s = '\0';
			} else {
				/* the output filename does not contain a path separator. It means that it contains only the filename */
				buffer[0] = '\0';
			}
			if (!strcmp(buffer, buffer2)) {
				/* the directory is equal before */
				continue;
			} else {
				create_missing_dirs(buffer);
				strcpy(buffer2, buffer);
			}
		}
	}
	
	/* return the lists */
	*_ls_input = ls_input;
	*_ls_output = ls_output;
}


								



void Program_Options_constructor(Program_Options* obj, const char* root_dir)
{
	char buffer[MAX_CHARACTERS_LINE];
	
	assert(obj);
	
	obj->root_path = string_clone(root_dir);
	
	sprintf(buffer, "%s%c%s", obj->root_path, SYSTEM_PATH_SEPARATOR, DEFAULT_TEST_DIR);
	obj->test_path = string_clone(buffer);
	
	obj->input_filename = NULL;
	obj->output_filename = NULL;
	obj->list_input_filename = NULL;
	obj->input_path = NULL;
	obj->output_path = NULL;
	
	sprintf(buffer, "%s%c%s", obj->root_path, SYSTEM_PATH_SEPARATOR, DEFAULT_DATA_DIR);
	obj->parameters_dir = string_clone(buffer);
	
	obj->output_overwriting = 1;
	
	IO_Type_constructor(&obj->load_concatenationLowLevelFeatures, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_CONCATENATIONLOWLEVELFEATURES);
	IO_Type_constructor(&obj->save_classemes, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_CLASSEMES);
	IO_Type_constructor(&obj->save_classemes_bin, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_CLASSEMESBIN);
	IO_Type_constructor(&obj->save_picodes128, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_PICODES128);
	IO_Type_constructor(&obj->save_picodes1024, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_PICODES1024);
	IO_Type_constructor(&obj->save_picodes2048, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_PICODES2048);
	IO_Type_constructor(&obj->save_mc, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_MC);
	IO_Type_constructor(&obj->save_mc_bin, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_MC_BIN);
	IO_Type_constructor(&obj->save_concatenationLowLevelFeatures, IOTYPE_NDEF_TYPE, DEFAULT_IOTYPE_MAT_VARNAME, DEFAULT_PREFIX_CONCATENATIONLOWLEVELFEATURES);
}



void Program_Options_destructor(Program_Options* obj)
{
	if (obj->root_path) free(obj->root_path);
	if (obj->test_path) free(obj->test_path);
	if (obj->input_filename) free(obj->input_filename);
	if (obj->output_filename) free(obj->output_filename);
	if (obj->list_input_filename) free(obj->list_input_filename);
	if (obj->input_path) free(obj->input_path);
	if (obj->output_path) free(obj->output_path);
	if (obj->parameters_dir) free(obj->parameters_dir);
}



int write_output(const char* file_name, const Matrix* M, const IO_Type* output_type, bool output_overwriting)
{
	int res;
	char buffer[MAX_CHARACTERS_LINE];
	
	assert(file_name);
	assert(M);
	
	if (output_type->type & IOTYPE_ASCII_TYPE) {
		sprintf(buffer, "%s"IOTYPE_ASCII_EXTENSION, file_name);
		if (!exist_file(buffer) || output_overwriting) {
			res = write_ascii_matrix(M, buffer);
			if (res) return res;
		}
	}
	if (output_type->type & IOTYPE_FLOAT_VLG_TYPE) {
		sprintf(buffer, "%s"IOTYPE_FLOAT_VLG_EXTENSION, file_name);
		if (!exist_file(buffer) || output_overwriting) {
			res = write_float_matrix(M, buffer);
			if (res) return res;
		}
	}
	if (output_type->type & IOTYPE_MAT_TYPE) {
#ifdef MAT_SUPPORT
		assert(output_type->MAT_varname);
		sprintf(buffer, "%s"IOTYPE_MAT_EXTENSION, file_name);
		if (!exist_file(buffer) || output_overwriting) {
			res = write_mat_matrix(M, buffer, output_type->MAT_varname);
			if (res) return res;
		}
#else
		assert(0);
#endif
	}
	if (output_type->type & IOTYPE_BIN_TYPE) {
		assert(is_vector(M));
		sprintf(buffer, "%s"IOTYPE_BIN_EXTENSION, file_name);
		if (!exist_file(buffer) || output_overwriting) {
			write_bit_vector(M, buffer);
		}
	}
	
	return 0;
}






int read_input(const char* file_name, Matrix* M, const IO_Type* input_type)
{
	int res;
	char buffer[MAX_CHARACTERS_LINE];
	
	assert(file_name);
	assert(M);
	assert(!(input_type->type & IOTYPE_ASCII_TYPE)); /* ASCII is not currently supported in reading */
	assert(!(input_type->type & IOTYPE_BIN_TYPE)); /* BIN is not currently supported in reading */
	
	if (input_type->type & IOTYPE_FLOAT_VLG_TYPE) {
		sprintf(buffer, "%s"IOTYPE_FLOAT_VLG_EXTENSION, file_name);
		res = read_float_matrix(M, buffer);
		return res;
	}
	if (input_type->type & IOTYPE_MAT_TYPE) {
#ifdef MAT_SUPPORT
		assert(input_type->MAT_varname);
		sprintf(buffer, "%s"IOTYPE_MAT_EXTENSION, file_name);
		res = read_mat_matrix(M, buffer, input_type->MAT_varname);
		return res;
#else
		assert(0);
#endif
	}
	
	/* we should not be here*/
	assert(0);
	return 0;
}






int exist_output(const char* file_name, const IO_Type* output_type)
{
	char buffer[MAX_CHARACTERS_LINE];
	
	assert(file_name);
	
	if (output_type->type & IOTYPE_ASCII_TYPE) {
		sprintf(buffer, "%s"IOTYPE_ASCII_EXTENSION, file_name);
		if (!exist_file(buffer)) {
			return 0;
		}
	}
	if (output_type->type & IOTYPE_FLOAT_VLG_TYPE) {
		sprintf(buffer, "%s"IOTYPE_FLOAT_VLG_EXTENSION, file_name);
		if (!exist_file(buffer)) {
			return 0;
		}
	}
	if (output_type->type & IOTYPE_MAT_TYPE) {
#ifdef MAT_SUPPORT
		sprintf(buffer, "%s"IOTYPE_MAT_EXTENSION, file_name);
		if (!exist_file(buffer)) {
			return 0;
		}
#else
		assert(0);
#endif
	}
	if (output_type->type & IOTYPE_BIN_TYPE) {
		sprintf(buffer, "%s"IOTYPE_BIN_EXTENSION, file_name);
		if (!exist_file(buffer)) {
			return 0;
		}
	}
	
	/* if we are here it means that all the output files exist */
	return 1;
}






