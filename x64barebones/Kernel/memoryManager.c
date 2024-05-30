#include "include/memoryManager.h"
#define MIN_BLOCK_SIZE 4096

#ifdef BUDDY

#define MAX_EXP 17
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
    if(bytes<MIN_BLOCK_SIZE){
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
#else

typedef struct Node{
    unsigned long long size;
    struct Node * next;
    struct Node * prev;
} Node;

Node * free_list = NULL;
Node * allocated_list = NULL;

void memInit(void* start_ptr, unsigned long size_bytes){
    if(size_bytes < MIN_BLOCK_SIZE ){
        return ;
    }


    Node* aligned_start_ptr = (Node*)start_ptr;
    if(((unsigned long long)aligned_start_ptr) % 8){
        aligned_start_ptr = aligned_start_ptr + (8 - ((unsigned long long)aligned_start_ptr)%8);
    }

    free_list = aligned_start_ptr;
    free_list->next = NULL;
    free_list->prev = NULL;
    free_list->size = size_bytes - sizeof(Node) ;
}

void * memAllocInner(unsigned long bytes){
    Node* current = free_list;
    Node* best = NULL;

    while (current != NULL) //Busco el Best fit
    {
        if((best == NULL || best->size > current->size) && current->size >= bytes){
            best = current;
        }
        current = current->next;
    }

    //Encontre uno, tengo que partirlo
    if(best != NULL){
        //Creo el nuevo nodo resultado de la particion y lo pongo en la freelist
        Node * newNode = (void *) best + bytes; //no uso sizeof(Node) porque bytes ya lo incluye
        newNode->next = best->next;
        newNode->prev = best->prev;
        newNode->size = best->size - bytes;
        best->size = bytes - sizeof(Node);

        //Saco al best de free_list
        if(newNode->next == NULL && newNode->prev == NULL){
            free_list = newNode;
        } else{
            if(newNode->prev != NULL){
                newNode->prev->next = newNode;
            }
            if(newNode->next != NULL){
                newNode->next->prev = newNode;
            }
        }

        //Inserto best en allocated_list
        best->prev = NULL;
        best->next = allocated_list;
        if(allocated_list != NULL){
            allocated_list->prev = best;
        }
        allocated_list = (void *)best;
    }
    return best;
}

/**
 *  Si un proceso pide 4097 bytes se le pasa un bloque de 4128: 4097 (solicitado) + 24 (header de nuevo nodo) + 7 (alineación)
 * @param bytes
 * @return
 */
void* memAlloc(unsigned long bytes){
    bytes += sizeof(Node);  //Agrego el tamaño del header para que entre el header de lo que sobre
                            //(aka el header del nuevo elemento de freelist)

    if(bytes < MIN_BLOCK_SIZE)
        bytes = MIN_BLOCK_SIZE;

    if(bytes % 8){
        bytes = bytes + (8 - bytes%8);  //Redondea a la proxima palabra
    }

    return memAllocInner(bytes) + sizeof(Node);
}

void memFreeInner(Node *node){
    if(node == NULL) return;

    //Saco a node de allocated_list
    if (node->prev == NULL && node->next == NULL){
        allocated_list = NULL;
    } else {
        if(node->prev != NULL){
            node->prev->next = node->next;
        }
        if(node->next != NULL){
            node->next->prev = node->prev;
        }
    }

    Node *current = free_list;
    //Busco el nodo que predeceria a node en free_list (ordenado por address) (o caso borde, node va al comienzo)
    while (current != NULL && current < node && current->next != NULL && current->next < node) {
        current = current->next;
    }
    //Encontre uno
    if(current != NULL && current < node){
        node->prev = current;
        node->next = current->next;
        if(current->next != NULL){
            current->next->prev = node;
        }
        current->next = node;
    } else {
        free_list = node;
        node->prev = NULL;
        node->next = current;
        if(current !=NULL){
            current->prev = node;
        }
    }

    //Chequeamos si podemos mergear con nuestro next
    if(node->next != NULL && node->next == (void *) node + sizeof(Node) + node->size){
        node->size += sizeof(Node) + node->next->size;
        if(node->next->next != NULL){
            node->next->next->prev = node;
        }
        node->next = node->next->next;
    }

    //Chequeamos si podemos mergear con nuestro prev
    if(node->prev != NULL && node == (void *) node->prev + sizeof(Node) + node->prev->size){
        node->prev->size += sizeof(Node) + node->size;
        if(node->next != NULL){
            node->next->prev = node->prev;
        }
        node->prev->next = node->next;
    }
}

void memFree(void *ptr){
    memFreeInner((Node*)(ptr - sizeof(Node)));    //el ptr q me pasan esta a sizeof(Node) del comienzo del bloque
}
#endif
