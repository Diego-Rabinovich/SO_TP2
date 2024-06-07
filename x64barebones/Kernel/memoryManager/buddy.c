#include "../include/memoryManager.h"
#define MIN_BLOCK_SIZE 1024
#define MAX_EXP 19
//Usamos unsigned long long porque las direcciones son de 8 bytes y tener void* en la macro no funcionaba
#define BLOCKSIZE(i) ((unsigned long long)(1 << (i)) * MIN_BLOCK_SIZE)
#define GET_BUDDY(b, i) ((((unsigned long long )(b)) ^ (BLOCKSIZE(i))))
typedef struct Block
{
    unsigned long long size;
    struct Block *next;
} Block;

static Block *free_lists[MAX_EXP + 1];
static void * firstAddress;

int obtainIndex(unsigned long size) {
    int i = 0;
    while (BLOCKSIZE(i) < size) {
        i++;
    }

    return i;
}

void memInit(void *start_ptr, unsigned long size_bytes){
    if(size_bytes < MIN_BLOCK_SIZE )
        return ;

    for (int i = 0; i < MAX_EXP + 1; i++)
        free_lists[i] = NULL;

    int i = obtainIndex(size_bytes);

    if(BLOCKSIZE(i) > size_bytes ){
        i--;
    }

    if (i > MAX_EXP)
    {
        i = MAX_EXP;
    }

    Block *aligned_start_ptr = (Block *)(((unsigned long long)start_ptr + 7) & ~7);
    firstAddress = aligned_start_ptr;
    free_lists[i] = aligned_start_ptr;
    free_lists[i]->next = NULL;
    free_lists[i]->size = BLOCKSIZE(i);
}

Block *memAllocRec(unsigned long bytes){
    int i = obtainIndex(bytes);


    if(i > MAX_EXP ){
        //NO HAY ESPACIO
        return NULL;
    }
    if(free_lists[i] != NULL){
        //ACA HAY UNO LIBRE
        Block *block = free_lists[i];
        free_lists[i] = free_lists[i]->next;
        block->size = BLOCKSIZE(i);
        return block;
    }
    Block *block = memAllocRec(BLOCKSIZE(i + 1)); // llamada recursiva para obtener un bloque mas grande
    if (block != NULL)
    {
        // Dividimos el bloque y ponemos el buddy en la lista de bloques libres
        unsigned long long addressDiffFromBase = (void *)block - firstAddress;
        Block *buddy = (Block *)(GET_BUDDY(addressDiffFromBase, i)  + (unsigned long long )firstAddress);
        buddy->size = BLOCKSIZE(i);
        buddy->next = free_lists[i];
        free_lists[i] = buddy;
        block->size = BLOCKSIZE(i);
    }
    return block;
}

void * memAlloc(unsigned long bytes){
    if(bytes==0){
        return NULL;
    }
    Block* ptr = memAllocRec(bytes + sizeof(Block));
    if(ptr){
        return (void*)(ptr + sizeof(Block));
    }
    return NULL;
}

void memFreeRec(void* ptr){
    unsigned long size = ((Block*)ptr)->size;
    int i = obtainIndex(size);

    unsigned long long addressDiffFromBase = ptr - firstAddress;
    Block* buddy = (Block *) (GET_BUDDY(addressDiffFromBase, i) + (unsigned long long ) firstAddress);
    Block** current = &free_lists[i];   //"iterador" de la lista

    //Busco si buddy esta libre
    while (*current != NULL && *current != buddy) {
        current = &((*current)->next);
    }

    // NO ESTA LIBRE BUDDY
    if (*current != buddy)
    {
        ((Block *)ptr)->next = free_lists[i];
        free_lists[i] = ptr;
    }
        // ESTA LIBRE BUDDY (HAY QUE MERGEAR)
    else
    {
        // Encontre el buddy, hago que lo que apuntaba a el apunte al siguiente
        *current = buddy->next;

        //Si ptr es la primera mitad, libero a ptr como el bloque entero, si no viceversa
        if (ptr > (void *)buddy)
        {
            buddy->size = BLOCKSIZE(i + 1);
            memFreeRec(buddy);
        }
        else
        {
            ((Block *)ptr)->size = BLOCKSIZE(i + 1);
            memFreeRec(ptr);
        }
    }


}
void memFree(void *ptr){
    memFreeRec(((Block *)ptr) - sizeof(Block));
}
