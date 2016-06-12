#include "memory.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

static mmAllocRoutine alloc_fp=malloc;
static mmFreeRoutine free_fp=free;
static mmObjectPtr integer_cache[65535];
static mmObjectPtr character_cache[255];

static mmObjectPtr new_character(char value);
static mmObjectPtr new_integer(long value);


void mm_init_memory_cache() {
    int i;
    int int_value;
    for (i=0;i<65535;i++) {
        int_value=i-32767;
        integer_cache[i]=new_integer(int_value);
        mm_inc_ref(integer_cache[i]);
    }
    for (i=0;i<255;i++) {
        character_cache[i]=new_character(i);
        mm_inc_ref(character_cache[i]);
    }
}

void mm_set_mem_routines(mmAllocRoutine ar,mmFreeRoutine fr)
{
	alloc_fp=ar;
	free_fp=fr;
}

void* mm_alloc(int words) {
	return alloc_fp(words);
}

void mm_free(void* block) {
	free_fp(block);
}

mmObjectPtr mm_new_object(mmObjectPtr cl,int num_slots) {
	mmObjectPtr no;
	int i;

	//no=mm_alloc(sizeof(mmObject)+((num_slots-1)*MM_WORD_SIZE));
	no=mm_alloc(sizeof(mmObject));
	no->_class=cl;
	no->_size=num_slots;
	no->_refcnt=0;
    no->_flags=0;
    if (num_slots > 0) {
        no->_slots=mm_alloc(sizeof(mmObjectPtr)*num_slots);
        for (i=0;i<num_slots;i++) {
            no->_slots[i]=mm_nil;
        }
    } else {
        no->_slots=NULL;
    }
	return no;
}

void mm_free_object(mmObjectPtr o) {
	int i;
	int num_slots=o->_size;
    mmObjectPtr clname;
	 
	 // temporary hack to send a release method to those
	 // objects that implement it.  Note that release should
	 // NOT be implemented at a generic base class level (i.e. Object)
	 // or any of the classes involved in sending messages, as
	 // this will result in a recursive loop
	 if (!mm_send_release(o)) {
		for (i=0;i<num_slots;i++) {
			mm_store_slot(o,i,mm_nil);
		}
	
  	  if ((o->_size > 0) && (o->_slots) ) {
  	      mm_free(o->_slots);
  	  }

		mm_free(o);
	 } else {
		o->_refcnt=-1000;
	}
}

void mm_nil_and_free_object(mmObjectPtr o) {
	int i;
	int num_slots=o->_size;
	 
		for (i=0;i<num_slots;i++) {
			mm_store_slot(o,i,mm_nil);
		}
	
  	  if ((o->_size > 0) && (o->_slots) ) {
  	      mm_free(o->_slots);
  	  }

		mm_free(o);
}

int mm_string_hash(char* s) {
    register char ch; 
    register int hash = 0; 
    char *p;

    p=s;
    while (ch = *p++) { 
        hash += ch; 
        hash += (hash << 8); 
    } 
    return abs(hash); 
} 

/* Integer special object routines */
mmObjectPtr mm_new_integer(long value) {
	mmInteger* i;

    if (    (value>=-32767) && (value<=32767)   ) {
        i=(mmInteger*)integer_cache[value+32767];
    } else {
        i=(mmInteger*)new_integer(value);
    }

	return (mmObjectPtr)i;
}

static mmObjectPtr new_integer(long value) {
	mmInteger* i;

    i=mm_alloc(sizeof(mmInteger));
    i->_class=mm_integer_class;
    i->_size=MM_INTEGER_SIZE;
    i->_refcnt=0;
    i->_flags=0;
    i->_intval=value;

	return (mmObjectPtr)i;
}

mmObjectPtr mm_new_integer_from_string(char* value) {
    int i;
    i=strtol(value,NULL,10);
    return mm_new_integer(i);
}

long mm_get_integer(mmObjectPtr o) {
	mmInteger* i;

	i=(mmInteger*)o;
	return i->_intval;
}

/* Float special object routines */
mmObjectPtr mm_new_float(double value) {
	mmFloat* f;

	f=mm_alloc(sizeof(mmFloat));
	f->_class=mm_float_class;
	f->_size=MM_FLOAT_SIZE;
	f->_refcnt=0;
    f->_flags=0;
	f->_floatval=value;

	return (mmObjectPtr)f;
}

mmObjectPtr mm_new_float_from_string(char* value) {
    double f;

    f=strtod(value,NULL);
    return mm_new_float(f);
}

double mm_get_float(mmObjectPtr o) {
	mmFloat* f;

	f=(mmFloat*)o;
	return f->_floatval;
}


mmObjectPtr mm_new_character(char value) {
    return character_cache[value];
}

static mmObjectPtr new_character(char value) {
    mmCharacter* c;

    c=mm_alloc(sizeof(mmCharacter));
	c->_class=mm_character_class;
	c->_size=MM_CHARACTER_SIZE;
	c->_refcnt=0;
    c->_flags=0;
	c->_charval=value;

	return (mmObjectPtr)c;
}

char mm_get_character(mmObjectPtr o) {
    mmCharacter* c;

    c=(mmCharacter*)o;
    return c->_charval;
}

mmObjectPtr mm_new_byte_array(mmObjectPtr cl,int size) {
	mmByteArray* no;
	int i;

	no=mm_alloc(sizeof(mmByteArray)+(size-1));
	no->_class=cl;
	no->_size=MM_BYTEARRAY_SIZE;
	no->_numbytes=size;
	no->_refcnt=0;
    no->_flags=0;
	// bzero the byte array
	bzero(&no->_bytes,size);
	return (mmObjectPtr)no;
}

mmObjectPtr mm_new_array(int size) {
    mmObjectPtr a;

    a=mm_new_object(mm_array_class,size);
    return a;
}


mmObjectPtr mm_new_string(char* str) {
    mmByteArray* no;

    no=(mmByteArray*)mm_new_byte_array(mm_string_class,strlen(str)+1);
    strcpy((char*)&no->_bytes,str);

    return (mmObjectPtr)no;
}

mmObjectPtr mm_new_fixed_len_string(char* str,int len) {
    mmByteArray* no;

    no=(mmByteArray*)mm_new_byte_array(mm_string_class,len);
    memcpy((char*)&no->_bytes,str,len);

    return (mmObjectPtr)no;
}

char* mm_get_string(mmObjectPtr str) {
    mmByteArray* no;

    no=(mmByteArray*)str;
    return (char*)no->_bytes;
}


mmObjectPtr mm_copy(mmObjectPtr o) {
	mmObjectPtr no;
	int i,size;

	size=o->_size;
	no=mm_new_object(o->_class,size);
	for (i=0;i<size;i++) {
		mm_store_slot(no,i,mm_get_slot(o,i));
	}

	return no;
}

mmObjectPtr mm_get_slot(mmObjectPtr o,int slot_num) {
	return o->_slots[slot_num];
}

void mm_store_slot(mmObjectPtr o,int slot_num,mmObjectPtr slot_value) {
	if (slot_value != mm_nil) {
		mm_inc_ref(slot_value);
	}
	if (o->_slots[slot_num] != mm_nil) {
		mm_dec_ref(o->_slots[slot_num]);
	}
	o->_slots[slot_num]=slot_value;	
}

void mm_inc_ref(mmObjectPtr o) {
	++o->_refcnt;
}

void mm_dec_ref(mmObjectPtr o) {
	if (!(--o->_refcnt)) {
		mm_free_object(o);
	}
}

char* mm_get_class_name(mmObjectPtr cl) {
    mmObjectPtr clName=mm_nil;
	char buff[256];

    bzero(buff,sizeof(buff));
    if (cl->_class==mm_metaclass_class) {
        clName = mm_get_slot(mm_get_slot(cl,MM_METACLASS_THISCLASS),MM_CLASS_NAME);
        strcat(buff,mm_get_string(clName));
        strcat(buff," class");
    } else {
        clName=mm_get_slot(cl,MM_CLASS_NAME);
        strcat(buff,mm_get_string(clName));
    }
	 return strdup(buff);
}

int mm_get_size(mmObjectPtr o) {
		  return o->_size;
}

mmObjectPtr mm_get_class(mmObjectPtr o) {	
		  return o->_class;
}

#ifdef MM_MEMORY_TEST_HARNESS
int main(int argc, char** argv) {
	mmObjectPtr x=mm_new_integer(1000);
	mmObjectPtr y=mm_new_integer(1000);
	mmObjectPtr p=mm_new_object(mm_nil,2);
	mmObjectPtr temp;


	mm_store_slot(p,0,x);
	mm_store_slot(p,1,y);
	temp=mm_get_slot(p,0);
	printf("temp is %d\n",mm_get_integer(temp));
    mm_free_object(p);
	return 0;
}
#endif
