#include "memory.h"
#include "primitive.h"

                             
static int _mm_characterAsInteger(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(mm_get_character(rcvr));
    return MM_PRIM_STD_RETURN;
}

static int _mm_characterValue(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_character(mm_get_integer(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}


static int _mm_char_equals(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=(mm_get_character(rcvr)==mm_get_character(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

void initcharacter(mmObjectPtr cl) {
    mmObjectPtr mc=mm_nil;

    static mmPrimMethod character_cl_primitives[]={
            {"asInteger",_mm_characterAsInteger},
            {"==",_mm_char_equals},
            {NULL,NULL}
    };
    static mmPrimMethod character_mc_primitives[]={
            {"value_1",_mm_characterValue},
            {NULL,NULL}
    };
    mc=cl->_class;
    mm_map_primitives(cl,character_cl_primitives);
    mm_map_primitives(mc,character_mc_primitives);
}


