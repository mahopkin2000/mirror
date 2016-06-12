#include "mirror.h"
#include "memory.h"
#include "dict.h"
#include "compiler.h"
#include "errno.h"
#include "unistd.h"
#include "string.h"
#include "interp.h"

#define MAX_COMPILE_ERRORS      10


/* globals */
mmObjectPtr mm_method_class=(mmObjectPtr)0;
mmObjectPtr mm_block_class=(mmObjectPtr)0;
mmObjectPtr mm_block_context_class=(mmObjectPtr)0;
mmObjectPtr mm_process_class=(mmObjectPtr)0;
mmObjectPtr mm_processor_class=(mmObjectPtr)0;
mmObjectPtr mm_dictionary_class = (mmObjectPtr)0;
mmObjectPtr mm_association_class = (mmObjectPtr)0;
mmObjectPtr mm_integer_class=(mmObjectPtr)0;
mmObjectPtr mm_float_class=(mmObjectPtr)0;
mmObjectPtr mm_array_class=(mmObjectPtr)0;  
mmObjectPtr mm_metaclass_class=(mmObjectPtr)0;
mmObjectPtr mm_cpointer_class=(mmObjectPtr)0;
mmObjectPtr mm_character_class=(mmObjectPtr)0;
mmObjectPtr mm_class_class=(mmObjectPtr)0;
mmObjectPtr mm_bytearray_class=(mmObjectPtr)0;
mmObjectPtr mm_string_class=(mmObjectPtr)0;
mmObjectPtr mm_undefinedobject_class=(mmObjectPtr)0;
mmObjectPtr mm_true_class=(mmObjectPtr)0;
mmObjectPtr mm_false_class=(mmObjectPtr)0;
mmObjectPtr mm_exception_class=(mmObjectPtr)0;


/* well known single instances (aka "The Dreaded Global Singleton") */
mmObjectPtr mm_nil=(mmObjectPtr)0;
mmObjectPtr mm_true=(mmObjectPtr)0;
mmObjectPtr mm_false=(mmObjectPtr)0;

/* system dictionary */
mmObjectPtr mm_sys_dict=(mmObjectPtr)0;

/* options */
int mm_debug = 0;
int mm_verbose = 0;
char*  mm_classPath = NULL;
char* mm_orig_wd = NULL;


int has_errors=0;
static int compile_error_count=0;


/* initialize global classes */
void mm_init() {
    mmObjectPtr core_dict;
    mmObjectPtr obj_class;
    mmObjectPtr coll_dict;
    mmObjectPtr system_class;
	 char buff[1024];

	 mm_orig_wd=getcwd(buff,sizeof(buff));
	 if (!mm_orig_wd) {
				error("Unable to call getcwd(%d): %s\n",errno,strerror(errno));
	 } else {
				mm_orig_wd=strdup(mm_orig_wd);
	 }

	mm_nil=mm_new_object(mm_undefinedobject_class,0);
    mm_true=mm_new_object(mm_true_class,0);
    mm_false=mm_new_object(mm_false_class,0);

    mm_integer_class=mm_new_object(mm_nil,-1000);
    mm_character_class=mm_new_object(mm_nil,-1000);
    mm_float_class=mm_new_object(mm_nil,-1000);
    mm_string_class=mm_new_object(mm_nil,-1000);
    mm_array_class=mm_new_object(mm_nil,-1000);
    mm_method_class=mm_new_object(mm_nil,-1000);
    mm_block_class=mm_new_object(mm_nil,-1000);
    mm_block_context_class=mm_new_object(mm_nil,-1000);
    mm_process_class=mm_new_object(mm_nil,-1000);
    mm_processor_class=mm_new_object(mm_nil,-1000);
    mm_dictionary_class=mm_new_object(mm_nil,-1000);
    mm_association_class=mm_new_object(mm_nil,-1000);
    mm_metaclass_class=mm_new_object(mm_nil,-1000);
    mm_cpointer_class=mm_new_object(mm_nil,-1000);
    mm_class_class=mm_new_object(mm_nil,-1000);
    mm_bytearray_class=mm_new_object(mm_nil,-1000);
    mm_undefinedobject_class=mm_new_object(mm_nil,-1000);
    mm_true_class=mm_new_object(mm_nil,-1000);  
    mm_false_class=mm_new_object(mm_nil,-1000);
	 mm_exception_class=mm_new_object(mm_nil,-1000);

    mm_init_memory_cache();

    mm_sys_dict=mm_new_dictionary(100);
    mm_inc_ref(mm_sys_dict);
    core_dict=mm_new_dictionary(100);
    coll_dict=mm_new_dictionary(100);
    mm_dict_put(mm_sys_dict,"Core",core_dict);
    mm_dict_put(mm_sys_dict,"Collection",coll_dict);
    mm_dict_put(core_dict,"Integer",mm_integer_class);
    mm_dict_put(core_dict,"Character",mm_character_class);
    mm_dict_put(core_dict,"Float",mm_float_class);
    mm_dict_put(coll_dict,"String",mm_string_class);
    mm_dict_put(coll_dict,"Array",mm_array_class);
    mm_dict_put(core_dict,"Method",mm_method_class);
    mm_dict_put(core_dict,"Block",mm_block_class);
    mm_dict_put(core_dict,"BlockContext",mm_block_context_class);
    mm_dict_put(core_dict,"Process",mm_process_class);
    mm_dict_put(core_dict,"Processor",mm_processor_class);
    mm_dict_put(coll_dict,"Dictionary",mm_dictionary_class);
    mm_dict_put(coll_dict,"Association",mm_association_class);
    mm_dict_put(core_dict,"Metaclass",mm_metaclass_class);
    mm_dict_put(core_dict,"CPointer",mm_cpointer_class);
    mm_dict_put(core_dict,"Class",mm_class_class);
    mm_dict_put(coll_dict,"ByteArray",mm_bytearray_class);
    mm_dict_put(core_dict,"UndefinedObject",mm_undefinedobject_class);
    mm_dict_put(core_dict,"True",mm_true_class);
    mm_dict_put(core_dict,"False",mm_false_class);
    mm_dict_put(core_dict,"Exception",mm_exception_class);


    mm_load_class("Core::Integer",NULL,mm_integer_class);
    mm_load_class("Core::Character",NULL,mm_character_class);
    mm_load_class("Core::Float",NULL,mm_float_class);
    mm_load_class("Collection::String",NULL,mm_string_class);
    mm_load_class("Collection::Array",NULL,mm_array_class);
    mm_load_class("Core::Method",NULL,mm_method_class);
    mm_load_class("Core::Block",NULL,mm_block_class);
    mm_load_class("Core::BlockContext",NULL,mm_block_context_class);
    mm_load_class("Core::Process",NULL,mm_process_class);
    mm_load_class("Core::Processor",NULL,mm_processor_class);
    mm_load_class("Collection::Dictionary",NULL,mm_dictionary_class);
    mm_load_class("Collection::Association",NULL,mm_association_class);
    mm_load_class("Core::Metaclass",NULL,mm_metaclass_class);
    mm_load_class("Core::CPointer",NULL,mm_cpointer_class);
    mm_load_class("Core::Class",NULL,mm_class_class);
    mm_load_class("Collection::ByteArray",NULL,mm_bytearray_class);
    mm_load_class("Core::UndefinedObject",NULL,mm_undefinedobject_class);
    mm_load_class("Core::True",NULL,mm_true_class);
    mm_load_class("Core::False",NULL,mm_false_class);
    mm_load_class("Core::Exception",NULL,mm_exception_class);

    /* fix up class Object Metaclass to have Class as the super class */
    obj_class=mm_dict_at(core_dict,"Object");
    mm_store_slot(obj_class->_class,MM_BEHAVIOR_SUPERCLASS,
                  mm_class_class);

    /* set system dictionary on the system class */
    system_class = mm_load_class("Core::System",NULL,mm_nil);
    mm_store_slot(system_class,MM_CLASS_SIZE,mm_sys_dict);

    mm_nil->_class=mm_undefinedobject_class;
    mm_true->_class=mm_true_class;
    mm_false->_class=mm_false_class;
    mm_inc_ref(mm_nil);
    mm_inc_ref(mm_true);
    mm_inc_ref(mm_false);
}

void error(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr,"ERROR: ");
    vfprintf(stderr,format,ap);
    va_end(ap);
    exit(-1);
}

void compile_error(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr,"ERROR: ");
    vfprintf(stderr,format,ap);
    va_end(ap);
    if (++compile_error_count > MAX_COMPILE_ERRORS) {
        fprintf(stderr,"Maximum number of compile errors exceeded - compilation aborted\n");
        exit(-1);
    }
    has_errors=1;
}

void debug(char* format, ...) {
    if (mm_verbose) {
        va_list ap;
        va_start(ap, format);
        (void) fprintf(stderr, "DEBUG: ");
        (void) vfprintf(stderr, format, ap);
        va_end(ap);
    }
}

