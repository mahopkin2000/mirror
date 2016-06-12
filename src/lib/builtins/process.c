#include "memory.h"
#include "primitive.h"
#include "interp.h"

// primitive methods for class Process                             
static int _mm_start(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    if (mm_get_integer(mm_get_slot(rcvr,MM_PROCESS_STATE))==MM_PROCESS_READY) {
        mm_store_slot(rcvr,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_RUNNING));
    }
    return MM_PRIM_STD_RETURN;
}

static int _mm_terminate(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_store_slot(rcvr,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_TERMINATED));
    return MM_PRIM_STD_RETURN;
}

static int _mm_suspend(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    if (mm_get_integer(mm_get_slot(rcvr,MM_PROCESS_STATE))==MM_PROCESS_RUNNING) {
        mm_store_slot(rcvr,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_SUSPENDED));
    }
    return MM_PRIM_STD_RETURN;
}

static int _mm_resume(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    if (mm_get_integer(mm_get_slot(rcvr,MM_PROCESS_STATE))==MM_PROCESS_SUSPENDED) {
        mm_store_slot(rcvr,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_RUNNING));
    }
    return MM_PRIM_STD_RETURN;
}

void initprocess(mmObjectPtr cl) {
    static mmPrimMethod process_primitives[]={
            {"start",_mm_start},
            {"terminate",_mm_terminate},
            {"suspend",_mm_suspend},
            {"resume",_mm_resume},
            {NULL,NULL}
    };
    mm_map_primitives(cl,process_primitives);
}

// primitive methods for class Semaphore
static int _mm_signal(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int excessSignals=mm_get_integer(mm_get_slot(rcvr,0));
    mmObjectPtr processList=mm_get_slot(rcvr,1);
    int size=mm_get_integer(mm_get_slot(processList,2));
    mmObjectPtr tail=mm_nil;
    mmObjectPtr process=mm_nil;
    
    if (size==0) {
        mm_store_slot(rcvr,0,mm_new_integer(++excessSignals));
    } else {
        // otherwise remove the process from the end of the process list
        // and resume it
        tail=mm_get_slot(processList,1);
        process=mm_get_slot(tail,2);
        _mm_resume(context,process,args,rv);
        if (size==1) {
            mm_store_slot(processList,0,mm_nil);
            mm_store_slot(processList,1,mm_nil);
        } else {
            mm_store_slot(processList,1,mm_get_slot(tail,0));
            mm_store_slot(mm_get_slot(tail,0),1,mm_nil); 
        }
        mm_store_slot(processList,2,mm_new_integer(--size));
    }
    return MM_PRIM_STD_RETURN;
}

static int _mm_wait(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int excessSignals=mm_get_integer(mm_get_slot(rcvr,0));
    mmObjectPtr processList=mm_get_slot(rcvr,1);
    int size=mm_get_integer(mm_get_slot(processList,2));
    mmObjectPtr activeProcess=getActiveProcess();
    mmObjectPtr link=mm_nil;
    mmObjectPtr head=mm_get_slot(processList,0);
    mmObjectPtr tail=mm_get_slot(processList,1);
    
    if (excessSignals > 0) {
        mm_store_slot(rcvr,0,mm_new_integer(--excessSignals));
    } else {
        link=mm_new_array(3);
        mm_store_slot(link,2,activeProcess);
        if (size==0) {
            head=tail=link;
        } else {
            mm_store_slot(link,0,tail);
            mm_store_slot(tail,1,link);
            tail=link;
        }
        mm_store_slot(processList,0,head);
        mm_store_slot(processList,1,tail);
        mm_store_slot(processList,2,mm_new_integer(++size));
        _mm_suspend(context,activeProcess,args,rv);
    }
        
    return MM_PRIM_STD_RETURN;
}

void initsemaphore(mmObjectPtr cl) {
    static mmPrimMethod semaphore_primitives[]={
            {"signal",_mm_signal},
            {"wait",_mm_wait},
            {NULL,NULL}
    };
    mm_map_primitives(cl,semaphore_primitives);
}



