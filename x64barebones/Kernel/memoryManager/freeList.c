#include "../include/memoryManager.h"
#define MIN_BLOCK_SIZE 128

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
        aligned_start_ptr = (Node*)(((unsigned long long)aligned_start_ptr + 7) & ~7);
    }

    free_list = aligned_start_ptr;
    free_list->next = NULL;
    free_list->prev = NULL;
    free_list->size = size_bytes - ((char*)aligned_start_ptr - (char*)start_ptr) - sizeof(Node);
}

void * memAllocInner(unsigned long bytes){
    Node* current = free_list;
    Node* best = NULL;

    while (current != NULL) // Busco el Best fit
    {
        if((best == NULL || best->size > current->size) && current->size >= bytes){
            best = current;
        }
        current = current->next;
    }

    // Encontre uno, tengo que partirlo
    if(best != NULL){
        // Creo el nuevo nodo resultado de la particion y lo pongo en la freelist
        Node * newNode = (Node*)((char*)best + bytes);
        newNode->size = best->size - bytes;
        newNode->next = best->next;
        newNode->prev = best->prev;

        best->size = bytes - sizeof(Node);

        // Sacar al best de free_list
        if(newNode->prev != NULL){
            newNode->prev->next = newNode;
        } else {
            free_list = newNode;
        }

        if(newNode->next != NULL){
            newNode->next->prev = newNode;
        }

        // Inserto best en allocated_list
        best->prev = NULL;
        best->next = allocated_list;
        if(allocated_list != NULL){
            allocated_list->prev = best;
        }
        allocated_list = best;
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

    // Saco a node de allocated_list
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        allocated_list = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    // Insert node back into free_list in address order
    Node *current = free_list;
    Node *prev = NULL;

    while (current != NULL && current < node) {
        prev = current;
        current = current->next;
    }

    node->next = current;
    node->prev = prev;
    if (current != NULL) {
        current->prev = node;
    }
    if (prev != NULL) {
        prev->next = node;
    } else {
        free_list = node;
    }

    // Merge with next block if adjacent
    if (node->next != NULL && (char*)node + sizeof(Node) + node->size == (char*)node->next) {
        node->size += sizeof(Node) + node->next->size;
        node->next = node->next->next;
        if (node->next != NULL) {
            node->next->prev = node;
        }
    }

    // Merge with previous block if adjacent
    if (node->prev != NULL && (char*)node->prev + sizeof(Node) + node->prev->size == (char*)node) {
        node->prev->size += sizeof(Node) + node->size;
        node->prev->next = node->next;
        if (node->next != NULL) {
            node->next->prev = node->prev;
        }
    }
}


void memFree(void *ptr){
    memFreeInner((Node*)(ptr - sizeof(Node)));    //el ptr q me pasan esta a sizeof(Node) del comienzo del bloque
}
