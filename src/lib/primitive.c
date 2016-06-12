#include "mirror.h"
#include "primitive.h"
#include "memory.h"
#include "stdio.h"
#include "stdlib.h"
#include "interp.h"
#include "dynload.h"
#include "dict.h"


/* initializers for built in primitives */
extern void initobject(mmObjectPtr cl);
extern void initfileconnection(mmObjectPtr cl);
extern void initstring(mmObjectPtr cl);
extern void initinteger(mmObjectPtr cl);
extern void initfloat(mmObjectPtr cl);
extern void initos(mmObjectPtr cl);
extern void initcharacter(mmObjectPtr cl);
extern void initblockcontext(mmObjectPtr cl);
extern void initfile(mmObjectPtr cl);
extern void initmirror(mmObjectPtr cl);
extern void initba(mmObjectPtr cl);
extern void initlargenumber(mmObjectPtr cl);
extern void initlargeinteger(mmObjectPtr cl);
extern void initlargefloat(mmObjectPtr cl);
extern void initprocess(mmObjectPtr cl);
extern void inittimestamp(mmObjectPtr cl);
extern void initsemaphore(mmObjectPtr cl);
extern void initsocket(mmObjectPtr cl);
extern void initsystem(mmObjectPtr cl);
extern void initweakref(mmObjectPtr cl);
extern void initregexp(mmObjectPtr cl);

static mmPrimInitializer mm_primInitializers[] = {
    {"Core::Object",initobject},
    {"IO::FileConnection",initfileconnection},
    {"Collection::String",initstring},
    {"Core::Integer",initinteger},
    {"Core::Float",initfloat},
    {"Core::OS",initos},
    {"Core::Character",initcharacter},
    {"Core::BlockContext",initblockcontext},
    {"IO::File",initfile},
    {"Collection::ByteArray",initba},
    {"Core::Process",initprocess},
    {"Core::TimeStamp",inittimestamp},
    {"Core::Semaphore",initsemaphore},
    {"Net::Socket",initsocket},
    {"Core::RegExp",initregexp},
//    {"Core::System",initsystem},
//    {"Core::WeakReference",initweakref},
    {NULL,NULL}
};

/*  attempt to initialize primitives for a class, using either static or
    dynamic linkage.  Could also allow a combination of the two which would
    allow dynamic linkage to override and/or augment static linkage.  This
    process is performed when the class is initially loaded and compiled */
void mm_init_primitives(char* namespace,char* sourceFile,mmObjectPtr cl,char* fn) {
    int idx;
    char* cn=mm_get_string(mm_get_slot(cl,MM_CLASS_NAME));
    char libname[128];
    char funcname[128];
    mmPrimInit pifp;
    char *p,*p1;
    FILE *existFP;
    char buff[512];
	 char libfile[512];
    

    for (idx=0;;idx++) {
        if (!mm_primInitializers[idx].cn) {
            break;
        }
        bzero(buff,sizeof(buff));
        sprintf(buff,"%s::%s",namespace,cn);
        if (strcmp(buff,mm_primInitializers[idx].cn)==0) {
            debug("initializing primitives for class %s\n",buff);
            mm_primInitializers[idx].fp(cl);
            break;
        }
    }
    
    /*  need to add code here to search for a dynamic library matching
        the classname (i.e. Object.so).  Then attempt to call a function
        in the library named init<classname>, where <classname> is the
        lowercase name of the class being loaded.  Note that this process
        is performed after any staticly linked primitive routines have
        been mapped for this class, which has the effect of allowing
        dynamically loaded primitive routines to override staticly loaded
        primitive routines.  This is intentional, to allow customization
        and/or modification of the standard primitive routines provided
        with the vanilla distribution of Mirror */ 
        
    bzero(libname,sizeof(libname));
    bzero(funcname,sizeof(funcname));
    bzero(libfile,sizeof(libfile));
    sprintf(libname,"%s.mmd",cn);
    sprintf(funcname,"init%s",cn);
    for (p=funcname;*p;*p=tolower(*p),p++);
    for (p=libname;*p;*p=tolower(*p),p++);
    
    // Get a handle to the DLL module.
/*
    debug("attempting to load dynamic library %s\n",libname);
	 sprintf(libfile,"%s%s%s",mm_orig_wd,MM_DIRECTORY_SEPARATOR,libname);
    pifp=mm_dynload_library(libfile,funcname,cl);
	 if (!pifp) {
    		bzero(libfile,sizeof(libfile));
		if (sourceFile[0]!=((char*)MM_DIRECTORY_SEPARATOR)[0]) {
				  sprintf(libfile,"%s%s%s",mm_orig_wd,MM_DIRECTORY_SEPARATOR,sourceFile);
		} else {
				  sprintf(libfile,"%s",sourceFile);
		}
		sprintf(libfile,"%s%s%s",dirname(libfile),MM_DIRECTORY_SEPARATOR,libname);

    	pifp=mm_dynload_library(libfile,funcname,cl);
	 }
	if (!pifp) {
    	pifp=mm_dynload_library(libname,funcname,cl);
	}
*/
}

void mm_map_primitives(mmObjectPtr cl,mmPrimMethod map[]) {
    mmObjectPtr md=mm_nil;
    char* methodName=NULL;
    void* fp=NULL;
    void* p=map;
    mmObjectPtr method=mm_nil;
    mmByteArray* fpba=(mmByteArray*)0;
    int idx=0;
    mmPrimMethod pm;
	 char* cn;

    md=mm_get_slot(cl,MM_BEHAVIOR_METHOD_DICT);
    while (1) {
        pm=map[idx++];
        methodName=pm.mn;
        fp=pm.fp;
        if (!methodName) {
            break;
        }
        method=mm_dict_at(md,methodName);
        if (method==mm_nil) {
            cn=mm_get_class_name(cl);
            error("Unable to map primitive routine for method %s::%s\n",cn,methodName);
        }
        fpba=(mmByteArray*)mm_new_byte_array(mm_cpointer_class,4);
        memcpy(fpba->_bytes,&fp,sizeof(fp));
        mm_store_slot(method,MM_NATIVE_METHOD_ADDRESS,(mmObjectPtr)fpba);
    }
}


mmObjectPtr mm_new_cpointer(void* ptr,char* type) {
    mmObjectPtr bytes=mm_new_byte_array(mm_bytearray_class,sizeof(void*));
    memcpy(((mmByteArray*)bytes)->_bytes,&ptr,sizeof(void*));
    mmObjectPtr cptr=mm_new_object(mm_cpointer_class,2);
	 mm_store_slot(cptr,MM_CPOINTER_BYTES,bytes);
	 mm_store_slot(cptr,MM_CPOINTER_TYPE,mm_new_string(type));
    return cptr;
}

void* mm_get_cpointer(mmObjectPtr cptr) {
    void* p;
    mmObjectPtr bytes=mm_get_slot(cptr,MM_CPOINTER_BYTES);
    memcpy(&p,((mmByteArray*)bytes)->_bytes,sizeof(p));
    return p;
}
