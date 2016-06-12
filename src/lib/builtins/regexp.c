#include "memory.h"
#include "primitive.h"
#include "interp.h"
#include "regex.h"

static mmObjectPtr _mm_getRegErrorString(int errcode, regex_t *compiled) {
    size_t length = regerror (errcode, compiled, NULL, 0);
    char *buffer = (char*)malloc(length);
    mmObjectPtr strObj=mm_nil;
    (void) regerror(errcode, compiled, buffer, length);
    strObj=mm_new_string(buffer);
    free(buffer);
    return strObj;
}

static int _mm_compile(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr pattern=mm_get_slot(args,0);
    mmObjectPtr flags=mm_get_slot(args,1);
    regex_t* re=(regex_t*)malloc(sizeof(regex_t));
    mmObjectPtr regex=mm_new_cpointer(re,"regex_t_p");
    int rc;
    rc=regcomp(re,mm_get_string(pattern),
               (mm_get_integer(flags)|REG_EXTENDED));
    if (rc==0) {
        mm_store_slot(rcvr,0,regex);
        return MM_PRIM_STD_RETURN;
    } else {
        free(re);
        return MM_PRIM_FAILURE;
    }
}

static int _mm_free(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr regex=mm_get_slot(rcvr,0);
    regex_t* re=mm_get_cpointer(regex);
    regfree(re);
    mm_store_slot(rcvr,0,mm_nil);
    return MM_PRIM_STD_RETURN;
}

static int _mm_match(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr regex=mm_get_slot(rcvr,0);
    mmObjectPtr str=mm_get_slot(args,0);
    regex_t* re=mm_get_cpointer(regex);
    int numSubExpr=re->re_nsub;
    regmatch_t* matchPtr=NULL;
    int rc;
    mmObjectPtr retArray=mm_nil;
    int arraySize=0;
    int i,ai;
    char* matchBuff;

    if (numSubExpr) {
        matchPtr=(regmatch_t*)malloc(sizeof(regmatch_t)*++numSubExpr);
    }
    rc=regexec (re,mm_get_string(str),numSubExpr,matchPtr,0);
    if (rc==REG_NOMATCH) {
        *rv=mm_nil;
        return MM_PRIM_STD_RETURN;
    } else if (rc==0) {
        // here need to return an array of matching subexpressions
        for (i=1;i<numSubExpr;i++) {
            if ( ( (matchPtr[i].rm_so<0) && (matchPtr[i].rm_eo<0) ) ||
                (matchPtr[i].rm_so==matchPtr[i].rm_eo) ) {
                continue;
            }
            ++arraySize;
        }
        retArray=mm_new_array(arraySize);
        for (i=1,ai=0;i<numSubExpr;i++) {
            if ( ( (matchPtr[i].rm_so<0) && (matchPtr[i].rm_eo<0) ) ||
                (matchPtr[i].rm_so==matchPtr[i].rm_eo) ) {
                continue;
            }
            matchBuff=(char*)
                malloc(matchPtr[i].rm_eo-matchPtr[i].rm_so+1);
            bzero(matchBuff,matchPtr[i].rm_eo-matchPtr[i].rm_so+1);
            memcpy(matchBuff,mm_get_string(str)+matchPtr[i].rm_so,
                matchPtr[i].rm_eo-matchPtr[i].rm_so);
            mm_store_slot(retArray,ai++,mm_new_string(matchBuff));
            free(matchBuff);
        }
        *rv=retArray;
        if (numSubExpr) {
            free(matchPtr);
        }
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

static int _mm_substitute(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    mmObjectPtr regex=mm_get_slot(rcvr,0);
    mmObjectPtr str=mm_get_slot(args,0);
    mmObjectPtr substr=mm_get_slot(args,1);
    regex_t* re=mm_get_cpointer(regex);
    int numSubExpr=re->re_nsub;
    regmatch_t* matchPtr=NULL;
    int rc;
    mmObjectPtr retArray=mm_nil;
    int arraySize=0;
    int i,ai;
    char* matchBuff;

    if (numSubExpr) {
        matchPtr=(regmatch_t*)malloc(sizeof(regmatch_t)*++numSubExpr);
    }
    rc=regexec (re,mm_get_string(str),numSubExpr,matchPtr,0);
    if (rc==REG_NOMATCH) {
        *rv=mm_nil;
        return MM_PRIM_STD_RETURN;
    } else if (rc==0) {
        // here need to return an array of matching subexpressions
        for (i=1;i<numSubExpr;i++) {
            if ( ( (matchPtr[i].rm_so<0) && (matchPtr[i].rm_eo<0) ) ||
                (matchPtr[i].rm_so==matchPtr[i].rm_eo) ) {
                continue;
            }
            ++arraySize;
        }
        retArray=mm_new_array(arraySize);
        for (i=1,ai=0;i<numSubExpr;i++) {
            if ( ( (matchPtr[i].rm_so<0) && (matchPtr[i].rm_eo<0) ) ||
                (matchPtr[i].rm_so==matchPtr[i].rm_eo) ) {
                continue;
            }
            matchBuff=(char*)
                malloc(matchPtr[i].rm_eo-matchPtr[i].rm_so+1);
            bzero(matchBuff,matchPtr[i].rm_eo-matchPtr[i].rm_so+1);
            memcpy(matchBuff,mm_get_string(str)+matchPtr[i].rm_so,
                matchPtr[i].rm_eo-matchPtr[i].rm_so);
            mm_store_slot(retArray,ai++,mm_new_string(matchBuff));
            free(matchBuff);
        }
        *rv=retArray;

        if (numSubExpr) {
            free(matchPtr);
        }
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

void initregexp(mmObjectPtr cl) {
    static mmPrimMethod regexp_primitives[]={
            {"free",_mm_free},
            {"match_1",_mm_match},
            {"compile_2",_mm_compile},
            {NULL,NULL}
    };
    mm_map_primitives(cl,regexp_primitives);
}


