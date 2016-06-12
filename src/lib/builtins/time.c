#include "memory.h"
#include "primitive.h"
#include "interp.h"
#include "time.h"
#include "sys/time.h"


static int _mm_getTimeOfDay(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int rc;
    struct timeval tv;
    rc=gettimeofday(&tv,NULL);
    if (!rc) {
        *rv=mm_new_array(2);
        mm_store_slot(*rv,0,mm_new_integer(tv.tv_sec));
        mm_store_slot(*rv,1,mm_new_integer(tv.tv_usec/1000));
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}


static int _mm_getTMStruct(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int rc;      
    struct tm tms;
    time_t t=mm_get_integer(mm_get_slot(args,0));
    localtime_r(&t,&tms);
    
    *rv=mm_new_array(9);
    mm_store_slot(*rv,0,mm_new_integer(tms.tm_year+1900));
    mm_store_slot(*rv,1,mm_new_integer(tms.tm_mon+1));
    mm_store_slot(*rv,2,mm_new_integer(tms.tm_mday));
    mm_store_slot(*rv,3,mm_new_integer(tms.tm_hour));
    mm_store_slot(*rv,4,mm_new_integer(tms.tm_min));
    mm_store_slot(*rv,5,mm_new_integer(tms.tm_sec));
    mm_store_slot(*rv,6,mm_new_integer(tms.tm_wday));
    mm_store_slot(*rv,7,mm_new_integer(tms.tm_yday));
    mm_store_slot(*rv,8,(tms.tm_isdst) ? mm_true : mm_false);

    return MM_PRIM_STD_RETURN;
}

void inittimestamp(mmObjectPtr cl) {
    mmObjectPtr mc=cl->_class;
    static mmPrimMethod timestamp_primitives[]={
            {NULL,NULL}
    };
    static mmPrimMethod timestamp_mc_primitives[]={
            {"getTimeOfDay",_mm_getTimeOfDay},
            {"getTimeStructure_1",_mm_getTMStruct},
            {NULL,NULL}
    };
    //mm_map_primitives(cl,timestamp_primitives);
    mm_map_primitives(mc,timestamp_mc_primitives);
}


