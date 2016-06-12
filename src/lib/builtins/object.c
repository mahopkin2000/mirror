#include "memory.h"
#include "primitive.h"
#include "interp.h"
#include "stdlib.h"

static int _mm_at(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int idx=mm_get_integer(mm_get_slot(args,0));
    *rv = mm_get_slot(rcvr,idx);
    return MM_PRIM_STD_RETURN;
}

static int _mm_put(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int idx=mm_get_integer(mm_get_slot(args,0));
    mm_store_slot(rcvr,idx,mm_get_slot(args,1));
    return MM_PRIM_STD_RETURN;
}

static int _mm_basicNew(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    long format=mm_get_integer(mm_get_slot(rcvr,MM_BEHAVIOR_FORMAT));
    *rv = mm_new_object(rcvr,format>>2);
    return MM_PRIM_STD_RETURN;
}

static int _mm_size(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(rcvr->_size);
    return MM_PRIM_STD_RETURN;
}

static int _mm_copy(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_copy(rcvr);
    return MM_PRIM_STD_RETURN;
}

/*
static int _mm_become(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    swapPointers(rcvr,mm_get_slot(args,0));
    return MM_PRIM_STD_RETURN;
}
*/

static int _mm_hash(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer((long)rcvr);
    return MM_PRIM_STD_RETURN;
}

static int _mm_getClass(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=rcvr->_class;
    return MM_PRIM_STD_RETURN;
}

static int _mm_identity(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv = (rcvr==mm_get_slot(args,0)) ? mm_true: mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_indexedNew(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    long format=mm_get_integer(mm_get_slot(rcvr,MM_BEHAVIOR_FORMAT));
    if (format & 0x01) {
        *rv=mm_new_byte_array(rcvr,mm_get_integer(mm_get_slot(args,0)));
    } else {
        *rv=mm_new_object(rcvr,mm_get_integer(mm_get_slot(args,0)));
    }
    return MM_PRIM_STD_RETURN;
}

static int _mm_perform(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int msrc;
    msrc=mm_msg_send(context,rcvr,mm_get_slot(args,0),mm_get_slot(args,1),0,rv);
    if (msrc==MM_MSG_SEND_PRIM_RETURN_OK) {
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_CONTROL;
    }
}

static int _mm_refcnt(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(rcvr->_refcnt);
    return MM_PRIM_STD_RETURN;
}

static int _mm_getFlags(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(rcvr->_flags);
    return MM_PRIM_STD_RETURN;
}

static int _mm_setFlags(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {	
	rcvr->_flags=mm_get_integer(mm_get_slot(args,0));
    return MM_PRIM_STD_RETURN;
}


void initobject(mmObjectPtr cl) {
    mmObjectPtr mc=mm_nil;
    static mmPrimMethod object_primitives[]={
        {"basicAt_1",_mm_at},
        {"basicPut_2",_mm_put},
        {"basicSize",_mm_size},
        {"copy",_mm_copy},
        /*{"become",_mm_become},*/
        {"hash",_mm_hash},
        {"getClass",_mm_getClass},
        {"==",_mm_identity},
        {"perform_2",_mm_perform},
        {"refcnt",_mm_refcnt},
        {"getFlags",_mm_getFlags},
        {"setFlags_1",_mm_setFlags},
        {NULL,NULL}
    };
    static mmPrimMethod object_cl_primitives[]={
        {"basicNew",_mm_basicNew},
        {"indexedNew_1",_mm_indexedNew},
        {NULL,NULL}
    };
    mm_map_primitives(cl,object_primitives);
    mc=cl->_class;
    mm_map_primitives(mc,object_cl_primitives);
}
