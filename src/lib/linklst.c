#include "mirror.h"
#include "linklst.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

mmLinkedList* mm_new_ll() {
    mmLinkedList* ll = (mmLinkedList*)malloc(sizeof(mmLinkedList));
    ll->_head = NULL;
    ll->_tail= NULL;
    ll->_size = 0;
    return ll;
}

void mm_add_ll_node(mmLinkedList* ll,void* data) {
    mmLinkedListNode* node = (mmLinkedListNode*)malloc(sizeof(mmLinkedListNode));
    node->_data= data;
    node->_next = NULL;
    if ((!ll->_head) && (!ll->_tail)) {
        ll->_tail = ll->_head = node;
    } else {
        ll->_tail->_next = node;
        ll->_tail = node;
    }
    ll->_size++;
}

void mm_store_ll_node(mmLinkedList* ll,int idx,void* data) {
    int i=0;
    mmLinkedListNode* n=ll->_head;

    for (i=0;i<idx;i++) {
        n=n->_next;
    }
    n->_data=data;

}

void mm_free_ll(mmLinkedList* ll) {
    mmLinkedListNode* node=ll->_head;
    mmLinkedListNode* temp;
    while (node) {
        temp = node;
        node = node->_next;
        free(temp);
    }
    free(ll);
}
