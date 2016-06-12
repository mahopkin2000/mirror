#include "memory.h"
#include "primitive.h"
#include "interp.h"
#include "compiler.h"

                             
static int _mm_value(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_IP,mm_new_integer(0));
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_SP,mm_new_integer(-1));
    mm_prepare_block(rcvr,mm_get_slot(args,0));
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_PARENT,context);
    mm_store_slot(getActiveProcess(),MM_PROCESS_CONTEXT,rcvr);
    return MM_PRIM_CONTROL;
}

static int _mm_newProcess(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_IP,mm_new_integer(0));
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_SP,mm_new_integer(-1));
    mm_prepare_block(rcvr,mm_get_slot(args,0));
    *rv=mm_schedule_process(rcvr);
    return MM_PRIM_STD_RETURN;
}

static int _mm_fork(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_IP,mm_new_integer(0));
    mm_store_slot(rcvr,MM_BLOCK_CONTEXT_SP,mm_new_integer(-1));
    mm_prepare_block(rcvr,mm_get_slot(args,0));
    *rv=mm_schedule_process(rcvr);
    mm_store_slot(*rv,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_RUNNING));
    return MM_PRIM_STD_RETURN;
}

static int _mm_doPrimitive(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr newrcvr=mm_get_slot(args,0);
    mmObjectPtr method=mm_get_slot(args,1);
    mmObjectPtr primArgs=mm_get_slot(args,2);
    mmObjectPtr primReturnArray=mm_get_slot(args,3);
    mmObjectPtr pmptr=mm_get_slot(method,MM_NATIVE_METHOD_ADDRESS);
    mmObjectPtr primreturn=0;
    int primrc=0;
    mmPrimFunc pmfptr=mm_get_cpointer(pmptr);

    if (!pmfptr) {
        error("primitive function pointer is NULL for method %s\n",mm_get_string(mm_get_slot(method,MM_METHOD_NAME)));
    }
    primrc=pmfptr(context,newrcvr,primArgs,&primreturn);
    if (primrc==MM_PRIM_STD_RETURN) {
        if (primreturn) {
            mm_store_slot(primReturnArray,0,primreturn);
        }
    }
    *rv=mm_new_integer(primrc);
    return MM_PRIM_STD_RETURN;
}

static int _mm_compile_block_in_context(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr ctxt=mm_get_slot(args,0);
    char* code=mm_get_string(mm_get_slot(args,1));
    *rv=mm_compile_anon_block(ctxt,code);
    return MM_PRIM_STD_RETURN;
}

void initblockcontext(mmObjectPtr cl) {
    mmObjectPtr mc=cl->_class;

    static mmPrimMethod blockcontext_primitives[]={
            {"value_1",_mm_value},
            {"newProcess_1",_mm_newProcess},
            {"fork_1",_mm_fork},
            {"doPrimitive_4",_mm_doPrimitive},
            {NULL,NULL}
    };

    static mmPrimMethod blockcontext_mc_primitives[]={
            {"compileBlockInContext_2",_mm_compile_block_in_context},
            {NULL,NULL}
    };

    mm_map_primitives(cl,blockcontext_primitives);
    mm_map_primitives(mc,blockcontext_mc_primitives);
}


