#include "memory.h"
#include "primitive.h"

#define CHECK_INT if (mm_get_slot(args,0)->_class!=mm_integer_class) {\
        return MM_PRIM_FAILURE;\
    }

static int _mm_integerAdd(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l=mm_get_integer(rcvr),r=mm_get_integer(mm_get_slot(args,0)),res;
    CHECK_INT;
    res=l+r;
    *rv=mm_new_integer(res);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerSub(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l=mm_get_integer(rcvr),r=mm_get_integer(mm_get_slot(args,0)),res;
    CHECK_INT;
    res=l-r;
    *rv=mm_new_integer(res);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerMult(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l=mm_get_integer(rcvr),r=mm_get_integer(mm_get_slot(args,0)),res;
    CHECK_INT;
    res=l*r;
    *rv=mm_new_integer(res);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerDiv(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l=mm_get_integer(rcvr),r=mm_get_integer(mm_get_slot(args,0)),res;
    CHECK_INT;
    res=l/r;
    *rv=mm_new_integer(res);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerMod(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_INT;
    *rv=mm_new_integer(mm_get_integer(rcvr) % mm_get_integer(mm_get_slot(args,0)));
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerLT(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_INT;
    *rv=(mm_get_integer(rcvr) < mm_get_integer(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerGT(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_INT;
    *rv=(mm_get_integer(rcvr) > mm_get_integer(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerEquals(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    CHECK_INT;
    *rv=(mm_get_integer(rcvr) == mm_get_integer(mm_get_slot(args,0))) ? mm_true : mm_false;
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerPrintString(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char buff[128];
    sprintf(buff,"%d",mm_get_integer(rcvr));
    *rv=mm_new_string(buff);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerBitAnd(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l,r;
    CHECK_INT;
    l=mm_get_integer(rcvr);
    r=mm_get_integer(mm_get_slot(args,0));
    *rv=mm_new_integer(l&r);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerBitOr(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l,r;
    CHECK_INT;
    l=mm_get_integer(rcvr);
    r=mm_get_integer(mm_get_slot(args,0));
    *rv=mm_new_integer(l|r);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerBitExclusiveOr(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l,r;
    CHECK_INT;
    l=mm_get_integer(rcvr);
    r=mm_get_integer(mm_get_slot(args,0));
    *rv=mm_new_integer(l^r);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerLeftShift(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l,r;
    CHECK_INT;
    l=mm_get_integer(rcvr);
    r=mm_get_integer(mm_get_slot(args,0));
    *rv=mm_new_integer(l<<r);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerRightShift(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    register long l,r;
    CHECK_INT;
    l=mm_get_integer(rcvr);
    r=mm_get_integer(mm_get_slot(args,0));
    *rv=mm_new_integer(l>>r);
    return MM_PRIM_STD_RETURN;
}

static int _mm_integerOnesComplement(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_new_integer(~(mm_get_integer(rcvr)));
    return MM_PRIM_STD_RETURN;
}

void initinteger(mmObjectPtr cl) {
    static mmPrimMethod integer_primitives[]={
            {"+",_mm_integerAdd},
            {"-",_mm_integerSub},
            {"*",_mm_integerMult},
            {"/",_mm_integerDiv},
            {"%",_mm_integerMod},
            {"<",_mm_integerLT},
            {">",_mm_integerGT},
            {"==",_mm_integerEquals},
            {"&",_mm_integerBitAnd},
            {"|",_mm_integerBitOr},
            {"^",_mm_integerBitExclusiveOr},
            {"<<",_mm_integerLeftShift},
            {">>",_mm_integerRightShift},
            {"onesComplement",_mm_integerOnesComplement},
            {"printString",_mm_integerPrintString},
            {NULL,NULL}
    };
    mm_map_primitives(cl,integer_primitives);
}


