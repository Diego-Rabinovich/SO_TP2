#include "stdint.h"
#include "linkedList.h"
#ifndef SO_TP2_SEMAPHORES_H
#define SO_TP2_SEMAPHORES_H
#define MAX_NAME 24

typedef struct  Semaphore{
    char name[MAX_NAME];
    int lock;
    uint32_t value;
    LinkedList queue;
    LinkedList lockQueue;
} Semaphore;

int32_t newSemaphore(char * name, uint32_t initial_value);
int8_t semIsOpen(char * name );
void semDestroy(char * name);
int8_t semPost(char * name);
int8_t semWait(char * name);


#endif //SO_TP2_SEMAPHORES_H
