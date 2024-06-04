#include "include/linkedList.h"
#include "include/memoryManager.h"

typedef struct LinkedListCDT {
    int len;
    Node *current;
    Node *first;
    Node *last;
} LinkedListCDT;

LinkedList createLinkedList(){
    LinkedList to_return = memAlloc(sizeof(LinkedListCDT));
    to_return->current = NULL;
    to_return->first = NULL;
    to_return->last = NULL;
    to_return->len = 0;
    return to_return;
}

void queue(LinkedList list, Node *node){
    if (list==NULL || node == NULL) return;

    if(list->last != NULL){
        list->last->next = node;
        node->prev = list->last;
    }else{
        list->first = node;
        list->first->prev = NULL;
    }
    list->last = node;
    list->last->next=NULL;
    (list->len)++;
}

void push(LinkedList list, Node *node){
    if (list==NULL || node == NULL) return;

    if(list->first != NULL){
        list->first->prev = node;
        node->next = list->first;
    }else{
        list->last = node;
        list->last->next = NULL;
    }
    list->first = node;
    list->first->prev=NULL;
    (list->len)++;
}

/**
 * Note that the node is not freed, if you want to free it use removeAndFree
 * @param list
 * @param node
 */
void remove(LinkedList list, Node *node){
    if (list==NULL || node == NULL) return;

    if(node->prev != NULL){
        node->prev->next = node->next;
    } else {
        list->first = node->next;
    }
    if (node->next != NULL){
        node->next->prev = node->prev;
    } else {
        list->last = node->prev;
    }

    list->len--;
}

void removeAndFree(LinkedList list, Node *node) {
    if (list==NULL || node == NULL) return;
    remove(list, node);
    memFree(node);
}

Node *peek(LinkedList list){
    if(list == NULL) return NULL;
    return list->first;
}

Node * pop(LinkedList list){
    Node * aux = peek(list);
    remove(list, aux);
    return aux;
}

int isEmpty(LinkedList list){
    if(list == NULL) return NULL;
    return list->len > 0;
}

void startIterator(LinkedList list){
    if(list == NULL) return;

    list->current = list->first;
}

int listHasNext(LinkedList list){
    if(list == NULL) return 0;

    return list->current != NULL;
}

void *listNext(LinkedList list){
    if (list != NULL && listHasNext(list)){
        Node * aux = list->current;
        list->current = list->current->next;
        return aux;
    }
    return NULL;
}

int getLength(LinkedList list){
    if(list == NULL) return 0;

    return list->len;
}

void freeList(LinkedList list){
    if(list == NULL) return;

    while(listHasNext(list)){
        Node * current = listNext(list);
        memFree(current);
    }

    memFree(list);
}