#ifndef _MM_PRIMITIVE_H_
#define _MM_PRIMITIVE_H_


#include "mirror.h"
#include "memory.h"

// primitive return constants
#define MM_PRIM_STD_RETURN      0
#define MM_PRIM_CONTROL         2
#define MM_PRIM_FAILURE         3

#define MM_CPOINTER_BYTES		0
#define MM_CPOINTER_TYPE		1


typedef int(*mmPrimFunc)(mmObjectPtr,mmObjectPtr,mmObjectPtr,mmObjectPtr*);
typedef void(*mmPrimInit)(mmObjectPtr);

typedef struct prim_method {
    char* mn;
    mmPrimFunc fp;
} mmPrimMethod;


typedef struct prim_initializer {
    char* cn;
    mmPrimInit fp;
} mmPrimInitializer;

/* interface */
#ifdef __cplusplus
extern "C" {
#endif

extern void mm_init_primitives(char* ns,char* sourceFile,mmObjectPtr cl,char* fn);
extern void mm_map_primitives(mmObjectPtr cl,mmPrimMethod map[]);
extern mmObjectPtr mm_new_cpointer(void* ptr,char* type);
extern void* mm_get_cpointer(mmObjectPtr cptr);

#ifdef __cplusplus
}
#endif


#endif
