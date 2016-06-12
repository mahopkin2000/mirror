#include "memory.h"
#include "primitive.h"

#ifdef NT
#include "windows.h"
#else
#include <unistd.h>
#endif

                             
static int _mm_delete(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
#ifdef NT
    DeleteFile(mm_get_string(mm_get_slot(args,0)));
#else    
    unlink(mm_get_string(mm_get_slot(args,0)));
#endif
    return 0;
}

static int _mm_exists(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=NULL;
    fp=fopen(mm_get_string(mm_get_slot(args,0)),"r");
    *rv=(fp) ? mm_true : mm_false;
    if (fp) {
        fclose(fp);
    }
    return 0;
}

void initfile(mmObjectPtr cl) {
    static mmPrimMethod file_primitives[]={
            {"primDelete_1",_mm_delete},
            {"primExists_1",_mm_exists},
            {NULL,NULL}
    };
    mm_map_primitives(cl,file_primitives);
}


