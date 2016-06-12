#include "memory.h"
#include "primitive.h"
#include "stdlib.h"
#include "errno.h"
#include <sys/types.h>
#include <sys/wait.h>

static mmObjectPtr mm_stdout=(mmObjectPtr)0;
static mmObjectPtr mm_stderr=(mmObjectPtr)0;
static mmObjectPtr mm_stdin=(mmObjectPtr)0;

static void mm_initCachedObjects() {
    mm_stdout=mm_new_cpointer(stdout,"FILE_p");
    mm_inc_ref(mm_stdout);
    mm_stderr=mm_new_cpointer(stderr,"FILE_p");
    mm_inc_ref(mm_stderr);
    mm_stdin=mm_new_cpointer(stdin,"FILE_p");
    mm_inc_ref(mm_stdin);
}

static int _mm_primStdoutHandle(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_inc_ref(mm_stdout);
    *rv=mm_stdout;
    return MM_PRIM_STD_RETURN;
}

static int _mm_primStderrHandle(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_inc_ref(mm_stderr);
    *rv=mm_stderr;
    return MM_PRIM_STD_RETURN;
}

static int _mm_primStdinHandle(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_inc_ref(mm_stdin);
    *rv=mm_stdin;
    return MM_PRIM_STD_RETURN;
}

static int _mm_exit(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    exit(mm_get_integer(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_system(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char* cmd=mm_get_string(mm_get_slot(args,0));
    int rc;
    rc=system(cmd);
    *rv=mm_new_integer(rc);
    return MM_PRIM_STD_RETURN;
}

static int _mm_getenv(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char* varname=mm_get_string(mm_get_slot(args,0));
    char* varval=NULL;
    varval=getenv(varname);
    *rv=(varval) ? mm_new_string(varval) : mm_nil;
    return MM_PRIM_STD_RETURN;
}

static int _mm_setenv(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char* varname=mm_get_string(mm_get_slot(args,0));
    char* varval=mm_get_string(mm_get_slot(args,1));
    int rc;
    rc=setenv(varname,varval,1);
    *rv=mm_new_integer(rc);
    return MM_PRIM_STD_RETURN;
}

static int _mm_errno(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(errno);
    return MM_PRIM_STD_RETURN;
}

static int _mm_strerror(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_string((char*)strerror(mm_get_integer(mm_get_slot(args,0))));
    return MM_PRIM_STD_RETURN;
}

static int _mm_fork(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int forkrc;
    forkrc=fork();
    *rv=mm_new_integer(forkrc);
    return MM_PRIM_STD_RETURN;
}

static int _mm_exec(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int execrc;
    char** execargs;
    int i;
    char* prog=mm_get_string(mm_get_slot(args,0));
    mmObjectPtr argarray=mm_get_slot(args,1);
    int argarraylen=argarray->_size;

    strcpy(prog,mm_get_string(mm_get_slot(args,0)));
    execargs=(char**)malloc(sizeof(char *) * (argarraylen+1));
    for (i=0;i<argarraylen;i++) {
        execargs[i]=(char*)mm_get_string(mm_get_slot(argarray,i));
    }
    execargs[argarraylen]=(char*)NULL;
    execrc=execvp((const char*)prog,(const char**)execargs);
    *rv=mm_new_integer(execrc);
    return MM_PRIM_STD_RETURN;
}

static int _mm_sleep(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int seconds=mm_get_integer(mm_get_slot(args,0));
    int sleeprv;
    sleeprv=sleep(seconds);
    *rv=mm_new_integer(sleeprv);
    return MM_PRIM_STD_RETURN;
}

static int _mm_alarm(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int seconds=mm_get_integer(mm_get_slot(args,0));
    int alarmrv;
    alarmrv=alarm(seconds);
    *rv=mm_new_integer(alarmrv);
    return MM_PRIM_STD_RETURN;
}

static int _mm_wait(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    pid_t pid;
    int status;
    pid=wait(&status);
    *rv=mm_new_array(2);
    mm_store_slot(*rv,0,mm_new_integer(pid));
    mm_store_slot(*rv,1,mm_new_integer(status));
    return MM_PRIM_STD_RETURN;
}

static int _mm_waitpid(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    pid_t retpid;
    int status;
    pid_t wpid=mm_get_integer(mm_get_slot(args,0));
    int options=mm_get_integer(mm_get_slot(args,1));
    
    retpid=waitpid(wpid,&status,options);
    *rv=mm_new_array(2);
    mm_store_slot(*rv,0,mm_new_integer(retpid));
    mm_store_slot(*rv,1,mm_new_integer(status));
    return MM_PRIM_STD_RETURN;
}

static int _mm_kill(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    pid_t pid=mm_get_integer(mm_get_slot(args,0));
    int signum=mm_get_integer(mm_get_slot(args,1));
    int rc;
    rc=kill(pid,signum);
    *rv=mm_new_integer(rc);
    return MM_PRIM_STD_RETURN;
}

void initos(mmObjectPtr cl) {
    mmObjectPtr mc=mm_nil;
    static mmPrimMethod os_primitives[]={
            {"primStdoutHandle",_mm_primStdoutHandle},
            {"primStderrHandle",_mm_primStderrHandle},
            {"primStdinHandle",_mm_primStdinHandle},
            {"exit_1",_mm_exit},
            {"system_1",_mm_system},
            {"getenv_1",_mm_getenv},
            {"setenv_2",_mm_setenv},
            {"errno",_mm_errno},
            {"strerror_1",_mm_strerror},
            {"fork",_mm_fork},
            {"exec_2",_mm_exec},
            {"sleep_1",_mm_sleep},
            {"alarm_1",_mm_alarm},
            {"wait",_mm_wait},
            {"waitpid_2",_mm_waitpid},
            {"kill_2",_mm_kill},
            {NULL,NULL}
    };
    mc=cl->_class;
    mm_map_primitives(mc,os_primitives);
    mm_initCachedObjects();
}

