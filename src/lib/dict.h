#ifndef _MM_DICT_H_
#define _MM_DICT_H_

#include "mirror.h"
#include "memory.h"
#include <stdarg.h>
#include <stdio.h>

#define MM_ASSOCIATION_SIZE     2
#define MM_DICTIONARY_SIZE      2


/* Association object routines */
extern mmObjectPtr mm_new_association(char* key,mmObjectPtr value);

/* Dictionary object routines */
extern mmObjectPtr mm_new_dictionary(int size);
extern mmObjectPtr mm_dict_at(mmObjectPtr dict,char* key);
extern void mm_dict_put(mmObjectPtr dict,char* key,mmObjectPtr value);
extern void mm_dict_print(mmObjectPtr dict);

#endif
