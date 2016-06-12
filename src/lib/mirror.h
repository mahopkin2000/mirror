#ifndef _MIRROR_H_
#define _MIRROR_H_

#include "config.h"

#define MM_PATH_SEPARATOR	":"
#define MM_DIRECTORY_SEPARATOR	"/"

/* forward declaration of object pointer */
typedef struct mm_object* mmObjectPtr;

/* global options */
extern int mm_debug;
extern int mm_verbose;
extern char* mm_classPath;
extern char* mm_orig_wd;


/* well known single instances (aka "The Dreaded Global Singleton") */
extern mmObjectPtr mm_nil;
extern mmObjectPtr mm_true;
extern mmObjectPtr mm_false;

/* well known class definitions */
extern mmObjectPtr mm_dictionary_class;
extern mmObjectPtr mm_association_class;
extern mmObjectPtr mm_method_class;
extern mmObjectPtr mm_block_class;
extern mmObjectPtr mm_block_context_class;
extern mmObjectPtr mm_process_class;
extern mmObjectPtr mm_processor_class;
extern mmObjectPtr mm_method_class;
extern mmObjectPtr mm_integer_class;
extern mmObjectPtr mm_float_class;
extern mmObjectPtr mm_array_class;
extern mmObjectPtr mm_string_class;
extern mmObjectPtr mm_metaclass_class;
extern mmObjectPtr mm_cpointer_class;
extern mmObjectPtr mm_character_class;
extern mmObjectPtr mm_class_class;
extern mmObjectPtr mm_bytearray_class;
extern mmObjectPtr mm_undefinedobject_class;
extern mmObjectPtr mm_true_class;
extern mmObjectPtr mm_false_class;
extern mmObjectPtr mm_exception_class;

/* system dictionary */
extern mmObjectPtr mm_sys_dict;

extern int has_errors;

/* interface */
extern void mm_init();

/* utility stuff */
extern void error(char *format, ...);
extern void debug(char* format, ...);
extern void compile_error(char *format, ...);


#endif
