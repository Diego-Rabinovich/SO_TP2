#include "include/processList.h"

PCB* PopProcess(QueueHeader* queue){
    if(queue == NULL || queue->first == NULL)
        return NULL;


    QueueNode* aux = queue->first;
    if(queue->first->next != NULL) {
        queue->first = queue->first->next;
        queue->last->next = aux;
        queue->last = aux;
        aux->next = NULL;
    }
    return aux->pcb;
}

void PushProcess(QueueHeader* queue, PCB* process_pcb){
    if(queue == NULL || process_pcb == NULL)
        return NULL;

    QueueNode* new_node = malloc(sizeof(QueueNode));
    new_node->pcb = process_pcb;
    new_node->next = NULL;

    (queue->count)++;

    if(first == NULL){
        queue->first = new_node;
        queue->last = new_node;
        queue->next = new_node;
        return;
    }
    queue->last->next = new_node;
    queue->last = new_node;
}

PCB* Iterate(QueueHeader* queue){
    if(queue == NULL || queue->next == NULL)
        return NULL;

    PCB* ret = queue->next->pcb;
    queue->next = queue->next->next;
    return ret;
}

uint32_t RestartIterate(QueueHeader* queue){
    if(queue == NULL)
        return -1;

    queue->next = queue->first;
    return queue->count;
}