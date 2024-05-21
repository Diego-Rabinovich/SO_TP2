#include "include/processList.h"

PCB* PopProcess(QueueHeader* queue){
    if(queue == NULL || queue->first == NULL)
        return NULL;


    QueueNode* aux = queue->first;
    if(queue->first->next != NULL) {
        queue->first = queue->first->next;
        queue->first->prev = NULL;

        queue->last->next = aux;
        aux->prev = queue->last;
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
    new_node->prev = NULL;

    (queue->count)++;

    if(first == NULL){
        queue->first = new_node;
        queue->last = new_node;
        queue->next = new_node;
        return;
    }
    new_node->prev = queue->last;
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

int RemoveProcess(QueueHeader * queue, uint32_t pid){
    QueueNode * current_node = queue->last;

    while(current_node != NULL){
        if(current_node->pcb.id == pid){
            if(current_node->prev != NULL){
                current_node->prev->next = current_node->next;
            }
            if(current_node->next != NULL){
                current_node->next->prev = current_node->prev;
            }

            free(current_node);
            return 1;
        }
    }
    return 0;
}

int TerminateProcess(QueueHeader * queue, uint32_t pid){
    QueueNode * current_node = queue->first;

    while(current_node != NULL){
        if(current_node->pcb.id == pid){
            current_node->pcb.p_state = TERMINATED;

            if(queue->first != current_node && current_node->prev != NULL) {
                current_node->prev->next = current_node->next;
                current_node->next = queue->first;
                queue->first = current_node;
                current_node->prev = NULL;
            }
            return 1;
        }
    }
    return 0;
}