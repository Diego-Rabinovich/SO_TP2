#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LinkedListCDT *LinkedList;

typedef struct Node {
    void* data;
    struct Node *prev;
    struct Node *next;
}Node;

LinkedList createLinkedList();
void queue(LinkedList list, Node *node);
void push(LinkedList list, Node *node);
void remove(LinkedList list, Node *node);
void removeAndFree(LinkedList list, Node * node);
Node *peek(LinkedList list);
Node *pop(LinkedList list);
int isEmpty(LinkedList list);
void startIterator(LinkedList list);
int listHasNext(LinkedList list);
void *listNext(LinkedList list);
int getLength(LinkedList list);

void freeList(LinkedList list);
#endif
