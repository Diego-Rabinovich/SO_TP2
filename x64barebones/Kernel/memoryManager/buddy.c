#include "../include/memoryManager.h"
#define MIN_BLOCK_SIZE 1024
#define MAX_EXP 19
#define BLOCKSIZE(i) ((unsigned long long)(1 << (i)) * MIN_BLOCK_SIZE)
#define GET_BUDDY(b, i) ((((unsigned long long )(b)) ^ (BLOCKSIZE(i))))

typedef struct Block {
    unsigned long long size;
    struct Block *next;
} Block;

static Block *free_lists[MAX_EXP + 1];
static void * firstAddress;
MemoryInfo mem_info;

int obtainIndex(unsigned long size) {
    int i = 0;
    while (BLOCKSIZE(i) < size) {
        i++;
    }
    return i;
}

void memInit(void *start_ptr, unsigned long size_bytes) {
    if(size_bytes < MIN_BLOCK_SIZE )
        return ;

    for (int i = 0; i < MAX_EXP + 1; i++)
        free_lists[i] = NULL;

    int i = obtainIndex(size_bytes);

    if(BLOCKSIZE(i) > size_bytes ){
        i--;
    }

    if (i > MAX_EXP) {
        i = MAX_EXP;
    }

    Block *aligned_start_ptr = (Block *)(((unsigned long long)start_ptr + 7) & ~7);
    firstAddress = aligned_start_ptr;
    free_lists[i] = aligned_start_ptr;
    free_lists[i]->next = NULL;
    free_lists[i]->size = BLOCKSIZE(i);
    mem_info.total = free_lists[i]->size;
    mem_info.free = free_lists[i]->size;
    mem_info.reserved =0;
}

Block *memAllocRec(unsigned long bytes) {
    int i = obtainIndex(bytes);

    if(i > MAX_EXP ){
        return NULL;
    }
    if(free_lists[i] != NULL){
        Block *block = free_lists[i];
        free_lists[i] = free_lists[i]->next;
        block->size = BLOCKSIZE(i);
        return block;
    }
    Block *block = memAllocRec(BLOCKSIZE(i + 1));
    if (block != NULL) {
        unsigned long long addressDiffFromBase = (void *)block - firstAddress;
        Block *buddy = (Block *)(GET_BUDDY(addressDiffFromBase, i)  + (unsigned long long )firstAddress);
        buddy->size = BLOCKSIZE(i);
        buddy->next = free_lists[i];
        free_lists[i] = buddy;
        block->size = BLOCKSIZE(i);
    }
    return block;
}

void * memAlloc(unsigned long bytes) {
    if(bytes==0){
        return NULL;
    }
    Block* ptr = memAllocRec(bytes + sizeof(Block));
    if(ptr){
        return (void*)(ptr + 1);
    }
    return NULL;
}

void memFreeRec(void* ptr) {
    unsigned long size = ((Block*)ptr)->size;
    int i = obtainIndex(size);

    unsigned long long addressDiffFromBase = ptr - firstAddress;
    Block* buddy = (Block *) (GET_BUDDY(addressDiffFromBase, i) + (unsigned long long ) firstAddress);
    Block** current = &free_lists[i];

    while (*current != NULL && *current != buddy) {
        current = &((*current)->next);
    }

    if (*current != buddy) {
        ((Block *)ptr)->next = free_lists[i];
        free_lists[i] = ptr;
    } else {
        *current = buddy->next;

        if (ptr > (void *)buddy) {
            buddy->size = BLOCKSIZE(i + 1);
            memFreeRec(buddy);
        } else {
            ((Block *)ptr)->size = BLOCKSIZE(i + 1);
            memFreeRec(ptr);
        }
    }
}

void memFree(void *ptr) {
    memFreeRec(((Block *)ptr) - 1);
}

MemoryInfo * getMemInfo(){
    uint64_t free = 0;
    for (int i = 0; i < MAX_EXP ; i++){
        Block * current = free_lists[i];
        while(current != NULL){
            free += current->size + sizeof (Block);
            current = current->next;
        }
    }
    mem_info.reserved = mem_info.total - free;
    mem_info.free = mem_info.free;

    return &mem_info;
}