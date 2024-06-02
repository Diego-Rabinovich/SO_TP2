#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LinkedListCDT *LinkedList;

typedef struct Node {
    void* data;
    struct Node *prev;
    struct Node *next;
}Node;

LinkedList createLinkedList();
Node *createElement(LinkedList list, void* data);
Node *queue(LinkedList list, Node *node);
Node *push(LinkedList list, Node *node);
void *remove(LinkedList list, Node *node);
Node *pop(LinkedList list);
int isEmpty(LinkedList list);
void startIterator(LinkedList list);
int hasNext(LinkedList list);
void *next(LinkedList list);
int getLength(LinkedList list);

void freeList(LinkedList list);
#endif
