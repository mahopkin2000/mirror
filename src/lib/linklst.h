#ifndef _MM_LINKED_LIST_
#define _MM_LINKED_LIST_

/* simple linked list implementation for storing growable lists of strings */
             
/* data structures */
typedef struct mm_linked_list_node {
    void* _data;
    struct mm_linked_list_node* _next;
} mmLinkedListNode;

typedef struct mm_linked_list {
    mmLinkedListNode* _head;
    mmLinkedListNode* _tail;
    int _size;
} mmLinkedList;


/* interface */
extern mmLinkedList* mm_new_ll();
extern void mm_add_ll_node(mmLinkedList* ll,void* data);
extern void mm_store_ll_node(mmLinkedList* ll,int idx,void* data);
extern void mm_free_ll(mmLinkedList* ll);

#endif
