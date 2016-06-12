#include "mirror.h"
#include "memory.h"
#include "compiler.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <getopt.h>
#include "interp.h"

#define MM_VERSION_OPT		0
#define MM_CLASSPATH_OPT	2
#define MM_VERBOSE_OPT		3
#define MM_HELP_OPT		4



static void usage();
static struct option longOptions[] = {
    {"version", 0, 0, MM_VERSION_OPT},
    {"classpath", 1, 0, MM_CLASSPATH_OPT},
    {"verbose", 0, 0, MM_VERBOSE_OPT},
    {"help", 0, 0, MM_HELP_OPT},
    {0, 0, 0, 0}
};
extern int has_errors;

int main(int argc, char** argv)
{
    FILE* infile;
    mmObjectPtr cl=mm_nil;
    char* entryMethod;
    char* classFile;
    char* className;
    int idx;
    mmObjectPtr args=mm_nil;
    mmObjectPtr argarray=mm_nil;
    int c;
    int optionIndex;
    mmObjectPtr rv=mm_nil;
    int i;
    mmObjectPtr system_class=mm_nil;

    while (1) {
        c = getopt_long(argc, argv, "",
                        longOptions, &optionIndex);
    	if (c == -1) {
    	    break;
        } else if (c == '?') {
            exit(-1);
        }
    	switch (c) {
    		case MM_VERSION_OPT:
    			fprintf(stderr,"Mirror version 1.0\n");
    			exit(-1);
    			break;
    		case MM_CLASSPATH_OPT:
    			mm_classPath = strdup(optarg);
    			break;
    		case MM_VERBOSE_OPT:
    			mm_verbose = 1;
    			break;
    		case MM_HELP_OPT:
    			usage();
    			exit(-1);
    			break;
            default:
                break;
    	}
    }  	
    if ((argc - optind) < 1) {
        usage();
        exit(-1);
    }

    classFile = strdup(argv[optind]);
    entryMethod = argv[optind+1];

    /* initialize the system */
    mm_init();

    className=strtok(classFile,".");
    cl=mm_load_class(className,NULL,mm_nil);
    system_class=mm_load_class("Core::System",NULL,mm_nil);
    if (has_errors) {
        fprintf(stderr,"Processing aborted due to compilation errors\n");
        exit(-1);
    }

    /* now need to locate an entry method in the specified class and run it */
    args=mm_new_array(argc-optind-1);
    for (idx=optind+1;idx<argc;idx++) {
        mm_store_slot(args,idx-optind-1,mm_new_string(argv[idx]));
    }
    argarray=mm_new_array(3);
    mm_store_slot(argarray,0,cl);
    mm_store_slot(argarray,1,mm_new_string("main"));
    mm_store_slot(argarray,2,args);
    mm_init_interpreter();
    mm_msg_send(mm_nil,system_class,mm_new_string("startSystem"),argarray,0,&rv);
    mm_interpret();
    mm_free_object(mm_get_slot(getActiveProcess(),MM_PROCESS_CONTEXT));
    
    return 0;
}

static void usage() {
    fprintf(stderr,"Usage: mirror [-options] <class file> [arguments . . .]\n");
    fprintf(stderr,"\t<class file> - source file for initial class to load.  This class\n");
    fprintf(stderr,"\tmust provide a class side method named main which accepts a single\n");
    fprintf(stderr,"\targument, an array of strings containing command line arguments\n");
    fprintf(stderr,"Where options include:\n");
    fprintf(stderr,"\t-version\tprint version of Mirror interpreter\n");
    fprintf(stderr,"\t-classpath\tdirectories seperated by %s, which constitute the search path for application classes\n",MM_PATH_SEPARATOR);
    fprintf(stderr,"\t-verbose\tprint debugging information regarding the state of the interpreter\n");
    fprintf(stderr,"\t-help\tprint this help message\n\n");
}

