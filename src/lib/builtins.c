#include "mirror.h"
#include "primitive.h"
#include "memory.h"


/* initializers for built in primitives */
extern void initsocket(mmObjectPtr cl);
extern void initregexp(mmObjectPtr cl);
extern void initsignal(mmObjectPtr cl);

static mmPrimInitializer mm_primInitializers[] = {
    {"Core::RegExp",initregexp},
    {"Net::Socket",initsocket},
    {"Core::Signal",initsignal},
    {NULL,NULL}
};

/*  attempt to initialize primitives for a class, using either static or
    dynamic linkage.  Could also allow a combination of the two which would
    allow dynamic linkage to override and/or augment static linkage.  This
    process is performed when the class is initially loaded and compiled */
void mm_init_extensions(char* namespace,mmObjectPtr cl,char* fn) {
    int idx;
    char* cn=mm_get_string(mm_get_slot(cl,MM_CLASS_NAME));
    char libname[128];
    char funcname[128];
    mmPrimInit pifp;
    char *p,*p1;
    FILE *existFP;
    char buff[512];
    

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
    
}
