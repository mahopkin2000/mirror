#include "memory.h"
#include "primitive.h"

static int _mm_primOpen(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char* fn=mm_get_string(mm_get_slot(args,0));
    char* fm=mm_get_string(mm_get_slot(args,1));
    FILE* fp=fopen(fn,fm);
    if (!fp) {
        return MM_PRIM_FAILURE;
    } else {
        *rv=mm_new_cpointer(fp,"FILE_p");
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_primClose(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    fclose(fp);
    return MM_PRIM_STD_RETURN;
}

static int _mm_primWriteFrom(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    char* str=mm_get_string(mm_get_slot(args,1));
    int pos=mm_get_integer(mm_get_slot(args,2));
    int bytes=mm_get_integer(mm_get_slot(args,3));
    if (fwrite(str+pos,bytes,1,fp)) {
        *rv=mm_new_integer(bytes);
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

static int _mm_primWriteChar(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    fputc(mm_get_character(mm_get_slot(args,1)),fp);
    return MM_PRIM_STD_RETURN;
}

static int _mm_primFlush(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    fflush(fp);
    return MM_PRIM_STD_RETURN;
}

static int _mm_primReadChar(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    char c;
    c=fgetc(fp);
    if (c>=0) {
        *rv=mm_new_character(c);
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

static int _mm_primSeek(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    if (fseek(fp,mm_get_integer(mm_get_slot(args,1)),SEEK_SET)) {
        return MM_PRIM_FAILURE;
    } else {
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_primReadInto(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    mmByte* ba=mm_get_raw_bytes(mm_get_slot(args,1));
    int pos=mm_get_integer(mm_get_slot(args,2));
    int bytes=mm_get_integer(mm_get_slot(args,3));
    clearerr(fp);
    if (fread(ba+pos,bytes,1,fp)) {
        *rv=mm_new_integer(bytes);
        return MM_PRIM_STD_RETURN;
    } else if (feof(fp)) {
        *rv=mm_nil;
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

static int _mm_primTell(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    FILE* fp=mm_get_cpointer(mm_get_slot(args,0));
    long pos;
    pos=ftell(fp);
    if (pos >= 0) {
        *rv=mm_new_integer(pos);
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

void initfileconnection(mmObjectPtr cl) {
    static mmPrimMethod fc_primitives[]={
        {"primOpen_2",_mm_primOpen},
        {"primClose_1",_mm_primClose},
        {"primWriteFrom_4",_mm_primWriteFrom},
        {"primWriteChar_2",_mm_primWriteChar},
        {"primFlush_1",_mm_primFlush},
        {"primReadChar_1",_mm_primReadChar},
        {"primSeek_2",_mm_primSeek},
        {"primReadInto_4",_mm_primReadInto},
        {"primTell_1",_mm_primTell},
        {NULL,NULL}
    };
    mm_map_primitives(cl,fc_primitives);
}
