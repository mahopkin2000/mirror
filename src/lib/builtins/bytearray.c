#include "memory.h"
#include "primitive.h"

                             
static int _mm_baAt(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int idx=mm_get_integer(mm_get_slot(args,0));
    mmByteArray* ba=(mmByteArray*)rcvr;
    *rv=mm_new_integer(ba->_bytes[idx]);
    return MM_PRIM_STD_RETURN;
}

static int _mm_baPut(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int idx=mm_get_integer(mm_get_slot(args,0));
    mmByteArray* ba=(mmByteArray*)rcvr;
    ba->_bytes[idx]=mm_get_integer(mm_get_slot(args,1));
    return MM_PRIM_STD_RETURN;
}

static int _mm_size(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmByteArray* ba=(mmByteArray*)rcvr;
    *rv=mm_new_integer(ba->_numbytes);
    return MM_PRIM_STD_RETURN;
}


void initba(mmObjectPtr cl) {
    static mmPrimMethod ba_primitives[]={
            {"at_1",_mm_baAt},
            {"put_2",_mm_baPut},
            {"size",_mm_size},
            {NULL,NULL}
    };
    mm_map_primitives(cl,ba_primitives);
}


