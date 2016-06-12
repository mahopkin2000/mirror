#include "memory.h"
#include "primitive.h"

#define CHECK_FLOAT if (mm_get_slot(args,0)->_class!=mm_float_class) {\
        return MM_PRIM_FAILURE;\
    }

                           
static int _mm_floatAdd(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=mm_new_float(mm_get_float(rcvr) + mm_get_float(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatSub(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=mm_new_float(mm_get_float(rcvr) - mm_get_float(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatMult(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=mm_new_float(mm_get_float(rcvr) * mm_get_float(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatDiv(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=mm_new_float(mm_get_float(rcvr) / mm_get_float(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatLT(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=(mm_get_float(rcvr) < mm_get_float(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatGT(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=(mm_get_float(rcvr) > mm_get_float(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatEquals(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_FLOAT;
    *rv=(mm_get_float(rcvr) == mm_get_float(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatPrintString(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char buff[128];
    sprintf(buff,"%f",mm_get_float(rcvr));
    *rv=mm_new_string(buff);
    return MM_PRIM_STD_RETURN;
}

static int _mm_floatFromValue(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr arg=mm_get_slot(args,0);
    mmObjectPtr cl=arg->_class;
    long lval;
    double dval;
    if ( (cl != mm_integer_class) &&
         (cl != mm_float_class) ) {
        return -1;
    }
    if (cl==mm_integer_class) {
        *rv=mm_new_float(mm_get_integer(arg));
    } else if (cl==mm_float_class) {
        mm_inc_ref(rcvr);
        *rv=rcvr;
    }
    return MM_PRIM_STD_RETURN;
}

static int _mm_asInteger(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
		  *rv=mm_new_integer((long)mm_get_float(rcvr));
    return MM_PRIM_STD_RETURN;
}

void initfloat(mmObjectPtr cl) {
    mmObjectPtr mc=mm_nil;
    static mmPrimMethod float_primitives[]={
            {"+",_mm_floatAdd},
            {"-",_mm_floatSub},
            {"*",_mm_floatMult},
            {"/",_mm_floatDiv},
            {"<",_mm_floatLT},
            {">",_mm_floatGT},
            {"=",_mm_floatEquals},
            {"printString",_mm_floatPrintString},
            {"asInteger",_mm_asInteger},
            {NULL,NULL}
    };
    static mmPrimMethod float_cl_primitives[]={
            {"fromValue_1",_mm_floatFromValue},
            {NULL,NULL}
    };
    mm_map_primitives(cl,float_primitives);
    mc=cl->_class;
    mm_map_primitives(mc,float_cl_primitives);
}


