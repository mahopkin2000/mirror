#include "memory.h"
#include "primitive.h"

 
                             
static int _mm_at(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int idx=mm_get_integer(mm_get_slot(args,0));
    mmByteArray* ba=(mmByteArray*)rcvr;
    *rv=mm_new_character(ba->_bytes[idx]);
    return MM_PRIM_STD_RETURN;
}

static int _mm_put(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int idx=mm_get_integer(mm_get_slot(args,0));
    mmByteArray* ba=(mmByteArray*)rcvr;
    ba->_bytes[idx]=mm_get_character(mm_get_slot(args,1));
    return MM_PRIM_STD_RETURN;
}

static int _mm_stringHash(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(mm_string_hash(mm_get_string(rcvr)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_stringEquals(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=(strcmp(mm_get_string(rcvr),mm_get_string(mm_get_slot(args,0)))==0) ? 
        mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}


static int _mm_asInteger(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer_from_string(mm_get_string(rcvr));
    return MM_PRIM_STD_RETURN;
}

static int _mm_asFloat(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_float_from_string(mm_get_string(rcvr));
    return MM_PRIM_STD_RETURN;
}

static int _mm_concat(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmByteArray *str1=(mmByteArray*)rcvr,*str2=(mmByteArray*)mm_get_slot(args,0);
    *rv=mm_new_byte_array(mm_string_class,str1->_numbytes+str2->_numbytes-1);
    strcat((char*)mm_get_raw_bytes(*rv),mm_get_raw_bytes(str1));
    strcat((char*)mm_get_raw_bytes(*rv),mm_get_raw_bytes(str2));
    return MM_PRIM_STD_RETURN;
}

void initstring(mmObjectPtr cl) {
    static mmPrimMethod string_primitives[]={
            {"at_1",_mm_at},
            {"put_2",_mm_put},
            {"hash",_mm_stringHash},
            {"=",_mm_stringEquals},
            {"asInteger",_mm_asInteger},
            {"asFloat",_mm_asFloat},
            {"primConcat_1",_mm_concat},
            {NULL,NULL}
    };
    mm_map_primitives(cl,string_primitives);
}


