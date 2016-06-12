#include "dict.h"



/* Association object routines */
mmObjectPtr mm_new_association(char* key,mmObjectPtr value) {
    mmObjectPtr assoc = mm_new_object(mm_association_class,MM_ASSOCIATION_SIZE);
    mm_store_slot(assoc,MM_ASSOCIATION_KEY_PTR,mm_new_string(key));
    mm_store_slot(assoc,MM_ASSOCIATION_VALUE_PTR,value);
    return assoc;
}

/*  Dictionary object routines.  Note that this simple implementation of a
    Dictionary assumes the keys are Strings.  Since Mirror does not have the
    concept of a Symbol like Smalltalk, comparison of keys is done using 
    strcmp rather than identity */
mmObjectPtr mm_new_dictionary(int size) {
    mmObjectPtr dict = mm_new_object(mm_dictionary_class,MM_DICTIONARY_SIZE);
    mm_store_slot(dict,MM_DICTIONARY_TALLY_PTR,mm_new_integer(0));
    if (!size) {
        size=2;
    }
    mm_store_slot(dict,MM_DICTIONARY_ARRAY_PTR,mm_new_array(size));
    return dict;
}

mmObjectPtr mm_dict_at(mmObjectPtr dict,char* key) {   
    mmObjectPtr array = mm_get_slot(dict,MM_DICTIONARY_ARRAY_PTR);
    int size = array->_size;
    int hash;
    mmObjectPtr rv = mm_nil;
    mmObjectPtr assoc,testkey;
    int i;

    if (size > 0) {
        hash = mm_string_hash(key) % size;
        for (i=hash;i < size;i++) {
            assoc = mm_get_slot(array,i);
            if (assoc==mm_nil) {
                return mm_nil;
            } else {
                testkey = mm_get_slot(assoc,MM_ASSOCIATION_KEY_PTR);
                if (strcmp(key,mm_get_string(testkey)) == 0) {
                    rv = mm_get_slot(assoc,MM_ASSOCIATION_VALUE_PTR);
                    break;
                }
            }
        }
    
        if (rv==mm_nil) {
            for (i=0;i < hash;i++) {
                assoc = mm_get_slot(array,i);
                if (assoc==mm_nil) {
                    return mm_nil;
                } else {
                    testkey = mm_get_slot(assoc,MM_ASSOCIATION_KEY_PTR);
                    if (strcmp(key,mm_get_string(testkey)) == 0) {
                        rv = mm_get_slot(assoc,MM_ASSOCIATION_VALUE_PTR);
                        break;
                    }
                }
            }
        }
    }

    return rv;    
}

void mm_dict_put(mmObjectPtr dict,char* key,mmObjectPtr value) {
    mmObjectPtr assoc = mm_nil;
    mmObjectPtr array = mm_get_slot(dict,MM_DICTIONARY_ARRAY_PTR);
    int size = array->_size;
    int hash;
    int i;
    mmObjectPtr testkey;
    int idx=-1;
    mmObjectPtr tally;

    if (size > 0) {
        hash = mm_string_hash(key) % size;
        for (i=hash;i < size;i++) {
            assoc = mm_get_slot(array,i);
            if (    (assoc==mm_nil) ||
                    (strcmp(key,mm_get_string(mm_get_slot(assoc,MM_ASSOCIATION_KEY_PTR)))==0) ) {
                idx=i;
                break;
            }
        }
    
        if (idx < 0) {
            for (i=0;i < hash;i++) {
                assoc = mm_get_slot(array,i);
                if (    (assoc==mm_nil) ||
                        (strcmp(key,mm_get_string(mm_get_slot(assoc,MM_ASSOCIATION_KEY_PTR)))==0) ) {
                    idx=i;
                    break;
                } 
            }
        } 
    }

    if (idx >= 0) {
        if (assoc==mm_nil) {
            assoc = mm_new_association(key,value);
            mm_store_slot(array,idx,assoc);
            tally=mm_get_slot(dict,MM_DICTIONARY_TALLY_PTR);
            mm_store_slot(dict,MM_DICTIONARY_TALLY_PTR,mm_new_integer(mm_get_integer(tally)+1));
        } else {
            mm_store_slot(assoc,MM_ASSOCIATION_VALUE_PTR,value);
        }
    } else {
        error("dictionary out of space error\n");
        exit(-1);
    }
}

void mm_dict_print(mmObjectPtr dict) {
    mmObjectPtr array = mm_get_slot(dict,MM_DICTIONARY_ARRAY_PTR);
    int size = array->_size;
    mmObjectPtr assoc,key,value;
    int i;

    debug("Dumping Dictionary of size %d:\n",size);
    for (i=0;i < size;i++) {
        assoc = mm_get_slot(array,i);
        if (assoc != mm_nil) {
            key = mm_get_slot(assoc,MM_ASSOCIATION_KEY_PTR);
            value = mm_get_slot(assoc,MM_ASSOCIATION_VALUE_PTR);
            debug("\tindex=%d,key=%s,value class ptr=%d\n",i,mm_get_string(key),
                    value);
        } else {
            debug("\tindex=%d - nil\n",i);
        }
    }
}


