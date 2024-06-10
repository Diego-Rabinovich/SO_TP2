#ifndef SO_TP2_MEMORYMANAGER_H
#define SO_TP2_MEMORYMANAGER_H

#include "stdint.h"
#define NULL 0x0000000


typedef struct MemoryInfo{
    uint64_t total;
    uint64_t reserved;
    uint64_t free;
} MemoryInfo;

void memInit(void* startPtr, unsigned long  sizeBytes);
void* memAlloc(unsigned long  bytes);
void memFree(void *ptr);
MemoryInfo * getMemInfo();

#endif //SO_TP2_MEMORYMANAGER_H
