#ifndef SO_TP2_MEMORYMANAGER_H
#define SO_TP2_MEMORYMANAGER_H

#define NULL 0x0000000

void memInit(void* startPtr, unsigned long  sizeBytes);
void* memAlloc(unsigned long  bytes);
void memFree(void *ptr);


#endif //SO_TP2_MEMORYMANAGER_H
