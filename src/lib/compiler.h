#ifndef _MM_COMPILER_H_
#define _MM_COMPILER_H_

#include "tree.h"
#include "memory.h"

/* interface */
extern mmObjectPtr mm_load_class(char* cn,mmAbstractNode* n,mmObjectPtr proto_class);
extern mmObjectPtr mm_compile_anon_block(mmObjectPtr context,char* code);
extern mmLinkedList* mm_get_classpath();
      
#endif
