#ifndef _MM_MEMORY_H_
#define _MM_MEMORY_H_

#include "mirror.h"
#include <stdarg.h>
#include <stdio.h>

/* #define MM_MEMORY_TEST_HARNESS 1 */

/* constants */
#define MM_WORD_SIZE        sizeof(mmWord)
#define MM_BYTES_PER_WORD   MM_WORD_SIZE
#define MM_HEADER_SIZE      (MM_WORD_SIZE * 3)
#define MM_INTEGER_SIZE		-1
#define MM_FLOAT_SIZE		-2 
#define MM_BYTEARRAY_SIZE	-3
#define MM_CHARACTER_SIZE   -4

/* Behavior fields */
#define MM_BEHAVIOR_METHOD_DICT     0
#define MM_BEHAVIOR_SUPERCLASS      1
#define MM_BEHAVIOR_FORMAT          2

/* ClassDescription fields */
#define MM_CLASSDESC_INSTVARS       3

/* Class fields */
#define MM_CLASS_NAME               4
#define MM_CLASS_POOL               5
#define MM_CLASS_NAMESPACE          6
#define MM_CLASS_SIZE               7

/* Metaclass fields */
#define MM_METACLASS_THISCLASS      4
#define MM_METACLASS_SIZE           5

/* well known field pointers */
#define MM_DICTIONARY_TALLY_PTR     0
#define MM_DICTIONARY_ARRAY_PTR     1
#define MM_ASSOCIATION_KEY_PTR      0
#define MM_ASSOCIATION_VALUE_PTR    1

/* object flags */
#define MM_IMPLEMENTS_RELEASE_FLAG	0x1

#define OBJ_HEADER \
	mmObjectPtr _class; \
	int _refcnt; \
	int _size; \
   int _flags; 
    


/* data structures */
typedef void* mmWord;
typedef unsigned char mmByte;

//typedef struct mm_object* mmObjectPtr;

typedef struct mm_object {
    OBJ_HEADER;
	mmObjectPtr* _slots;
} mmObject;	

typedef struct mm_integer {
    OBJ_HEADER;
	long _intval;
} mmInteger;

typedef struct mm_float {
    OBJ_HEADER;
	double _floatval;
} mmFloat;

typedef struct mm_character {
    OBJ_HEADER;
	char _charval;
} mmCharacter;

typedef struct mm_byte_array {
    OBJ_HEADER;
	int _numbytes;
	mmByte _bytes[1];
} mmByteArray;

/* helper macro for getting at raw bytes of a byte array */
#define mm_get_raw_bytes(bytearray) \
    ((mmByteArray*)bytearray)->_bytes

typedef void(*mmFreeRoutine)(void*);
typedef void*(*mmAllocRoutine)(size_t);

#ifdef __cplusplus
extern "C" {
#endif

/* low level memory allocation */
extern void mm_set_mem_routines(mmAllocRoutine ar,mmFreeRoutine fr);
extern void* mm_alloc(int words);
extern void mm_free(void* block);
extern mmObjectPtr mm_new_object(mmObjectPtr cl,int num_slots);
extern void mm_free_object(mmObjectPtr o);
extern void mm_nil_and_free_object(mmObjectPtr o);
extern mmObjectPtr mm_copy(mmObjectPtr o);
extern int mm_string_hash(char* s);
extern mmObjectPtr mm_get_slot(mmObjectPtr o,int slot_num);
extern void mm_store_slot(mmObjectPtr o,int slot_num,mmObjectPtr slot_value);
extern void mm_inc_ref(mmObjectPtr o);
extern void mm_dec_ref(mmObjectPtr o);
extern int mm_get_size(mmObjectPtr o);
extern mmObjectPtr mm_get_class(mmObjectPtr o);
char* mm_get_class_name(mmObjectPtr cl);

/* Integer special object routines */
extern mmObjectPtr mm_new_integer(long value);
extern mmObjectPtr mm_new_integer_from_string(char* value);
extern long mm_get_integer(mmObjectPtr o);

/* Float special object routines */
extern mmObjectPtr mm_new_float(double value);
extern mmObjectPtr mm_new_float_from_string(char* value);
extern double mm_get_float(mmObjectPtr o);

/* Character special object routines */
extern mmObjectPtr mm_new_character(char value);
extern char mm_get_character(mmObjectPtr o);

/* byte object routines */
extern mmObjectPtr mm_new_byte_array(mmObjectPtr cl,int size);

/* Array routines */
extern mmObjectPtr mm_new_array(int size);

/* String routines */
extern mmObjectPtr mm_new_string(char* str);
extern mmObjectPtr mm_new_fixed_len_string(char* str,int len);
extern char* mm_get_string(mmObjectPtr str);

extern void mm_init_memory_cache();
#ifdef __cplusplus
}
#endif


#endif

