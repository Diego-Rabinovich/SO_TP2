#include "include/semaphores.h"
#include "lib.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "videoDriver.h"
#define MAX_SEMS 1024

Semaphore * sems[MAX_SEMS] = {0};
uint32_t semCount = 0;
uint32_t nextID = 0;

static void setNextIdx(){
    nextID = 0;
    while(sems[nextID] != NULL && nextID < MAX_SEMS){
        nextID++;
    }
}

static int getSem(char * name){
    for (int i = 0; i < MAX_SEMS; i++){
        if (sems[i] != NULL && strCmp(sems[i]->name, name) == 0){
            return i;
        }
    }
    return -1;
}

static void releaseFirstP(LinkedList queue){
    Node * current;
    while((current = pop(queue)) != NULL){
        uint16_t pid = (uint16_t) ((uint64_t) current->data);
        memFree(current);
        if (getPState(pid) != TERMINATED){
            setState(pid, READY);
            break;
        }
    }
}

static void releaseLock(Semaphore * sem){
    releaseFirstP(sem->lockQueue);
    sem->lock = 0;
}

static void acquireLock(Semaphore * sem){
    while (xchg(&(sem->lock), 1)) {
        uint16_t pid = getPid();
        Node * node = memAlloc(sizeof (Node));
        node->data = (void *) ((uint64_t ) pid);
        queue(sem->lockQueue, node);
        setState(pid, BLOCKED);
    }
}

int32_t newSemaphore(char * name , uint32_t initial_value){
    int nameLen = strLen(name);
    if (getSem(name) != -1){
        return 0;
    }
    if ( semCount >= MAX_SEMS || nameLen >= MAX_NAME ){
        return -1;
    }
    sems[nextID] = memAlloc(sizeof (Semaphore));
    memcpy(sems[nextID]->name, name, nameLen+1);
    sems[nextID]->lock = 0;
    sems[nextID]->lockQueue = createLinkedList();
    sems[nextID]->queue = createLinkedList();
    sems[nextID]->value = initial_value;
    semCount++;
    setNextIdx();
    return 0;
}

int8_t semIsOpen(char * name ){
    return (int8_t)(getSem(name) != -1);
}

void semDestroy(char * name){
    int auxIdx = getSem(name);
    if (auxIdx != -1){
        Semaphore * aux = sems[auxIdx];
        freeList(aux->queue);
        freeList(aux->lockQueue);
        memFree(aux);
        sems[auxIdx] = NULL;
        semCount--;
    }
}

int8_t semPost(char * name){
    int semIdx = getSem(name);
    if (semIdx == -1) return -1;
    Semaphore  * sem = sems[semIdx];

    acquireLock(sem);
    sem->value++;

    releaseFirstP(sem->queue);
    releaseLock(sem);
    return 0;
}

int8_t semWait(char * name){
    int semIdx = getSem(name);
    if (semIdx == -1) return -1;
    Semaphore  * sem = sems[semIdx];

    acquireLock(sem);

    while (sem->value == 0) {
        uint16_t pid = getPid();
        Node * node = memAlloc(sizeof (Node));
        node->data = (void *) ((uint64_t) pid);
        queue(sem->queue, node);
        releaseLock(sem);
        setState(pid, BLOCKED);
        acquireLock(sem);
    }
    sem->value--;

    releaseLock(sem);

    return 0;
}

