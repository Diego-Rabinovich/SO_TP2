#include "include/memoryManager.h"

char* mem;
char* memPtr;
unsigned long  size;

void memInit(void* startPtr, unsigned long  sizeBytes){
    mem= (char*)startPtr;
    memPtr = mem;
    size=sizeBytes;
}

void* memAlloc(unsigned long  bytes){
    if(memPtr + bytes > mem + size)
        return NULL;

    void* ret = memPtr;
    memPtr+=bytes;
    return ret;
}

void* memCalloc(unsigned long  units, unsigned long  unitSize){
    unsigned long  bytes = units*unitSize;
    if(memPtr + bytes > mem + size)
        return NULL;

    void* ret = memPtr;
    for (; memPtr < (char*)ret + bytes; ++memPtr) {
        *memPtr=0;
    }

    return ret;
}
