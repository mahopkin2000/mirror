#include "dynload.h"

#ifdef _NT_
#include "windows.h"
#else
#include "dlfcn.h"
#endif 

#include <stdio.h>
#include "primitive.h"

extern void* mm_dynload_library(char* libname,char* symbol,mmObjectPtr cl) {
#ifdef _NT_
    HINSTANCE hinstLib; 
    mmPrimInit pifp=NULL;
    
    debug("attempting to load dynamic library %s\n",libname);
    hinstLib = LoadLibrary(libname); 
    
    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL) { 
      debug("loaded dynamic library %s,attempting to lookup init function %s\n",\
             libname,symbol);
      pifp = (mmPrimInit) GetProcAddress(hinstLib, symbol); 
    }

    return pifp;
#else
    void *library;
    mmPrimInit pifp=NULL;
    const char *error;
    
    debug("attempting to load dynamic library %s\n",libname);
    library = dlopen(libname, RTLD_LAZY);
    if (library != NULL) {
        debug("loaded dynamic library %s,attempting to lookup init function %s\n",\
               libname,symbol);
        pifp = dlsym(library, symbol);
        if (!pifp) {
            error = dlerror();
            if (error) {
              debug("Could not find %s: %s\n", symbol,error);
            }     
        } else {
            debug("calling initialization function %s in dynamic library %s\n",
                 symbol,libname);
            pifp(cl);
        }
        //dlclose(library);
    } else {
        error=dlerror();
        if (error) {
          debug("Could not open library %s: %s\n", libname,error);
        }     
    }

    return pifp;
#endif
}
