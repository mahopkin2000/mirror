#include "memory.h"
#include "primitive.h"
#include "interp.h"
#include "dict.h"
#include <signal.h>
#include "compiler.h"

static volatile sig_atomic_t insideHandler=0;
static mmObjectPtr _mm_signalClass=(mmObjectPtr)0;
static mmObjectPtr _mm_signalSelector=(mmObjectPtr)0;
static mmObjectPtr _mm_signalArgs=(mmObjectPtr)0;

// this handler is used for all signals in which the mirror program
// has registered an interest.  It dispatches control to a method 
// on the Signal class, which then performs a lookup in the handlers
// Dictionary and invokes the closure for that handler if one is
// registered.  Note that signals are blocked during interpreter cycles 
// to avoid corruption of the internal state of the interpreter.  This
// is certainly not the most efficient or responsive signal handling
// strategy, but it seemed the easiest at the time
static void _mm_handleSignal(int sig) {
    volatile mmObjectPtr context=mm_nil;
    volatile mmObjectPtr msgSendRV=mm_nil;
    if (insideHandler)
        raise(sig);
    insideHandler=1;
    context=mm_get_slot(getActiveProcess(),MM_PROCESS_CONTEXT);
    mm_store_slot(_mm_signalArgs,0,mm_new_integer(sig));
    mm_msg_send(context,_mm_signalClass,_mm_signalSelector,
        _mm_signalArgs,0,(mmObjectPtr*)&msgSendRV);
    /*
    mm_store_slot(
        mm_get_slot(getActiveProcess(),MM_PROCESS_CONTEXT),
        MM_BLOCK_CONTEXT_FLAGS,
        mm_new_integer(1));
        */
    insideHandler=0;
}
                             
static int _mm_loadSignalConstants(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_dictionary(100);
    mm_dict_put(*rv,"SIGINT",mm_new_integer(SIGINT));
    mm_dict_put(*rv,"SIGTERM",mm_new_integer(SIGTERM));
    mm_dict_put(*rv,"SIGUSR1",mm_new_integer(SIGUSR1));
    mm_dict_put(*rv,"SIGUSR2",mm_new_integer(SIGUSR2));
    mm_dict_put(*rv,"SIGALRM",mm_new_integer(SIGALRM));
    mm_dict_put(*rv,"SIGCHLD",mm_new_integer(SIGCHLD));
    return MM_PRIM_STD_RETURN;
}

static int _mm_primRegisterHandler(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int signum=mm_get_integer(mm_get_slot(args,0));
    signal(signum,_mm_handleSignal);
    return MM_PRIM_STD_RETURN;
}

static int _mm_primUnregisterHandler(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int signum=mm_get_integer(mm_get_slot(args,0));
    signal(signum,SIG_DFL);
    return MM_PRIM_STD_RETURN;
}

static int _mm_raise(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int signum=mm_get_integer(mm_get_slot(args,0));
    raise(signum);
    return MM_PRIM_STD_RETURN;
}


void initsignal(mmObjectPtr cl) {
    mmObjectPtr mc=cl->_class;
    static mmPrimMethod process_mc_primitives[]={
            {"loadSignalConstants",_mm_loadSignalConstants},
            {"primRegisterHandler",_mm_primRegisterHandler},
            {"primUnregisterHandler",_mm_primUnregisterHandler},
            {"raise",_mm_raise},
            {NULL,NULL}
    };
    mm_map_primitives(mc,process_mc_primitives);
    _mm_signalClass=mm_load_class("Core::Signal",NULL,mm_nil);
    _mm_signalSelector=mm_new_string("signalReceived");
    _mm_signalArgs=mm_new_array(1);
}


