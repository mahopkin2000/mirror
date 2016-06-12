#ifndef _MM_DYNLOAD_H_
#define _MM_DYNLOAD_H_

#include "mirror.h"

extern void* mm_dynload_library(char* libname,char* symbol,mmObjectPtr cl);

#endif
