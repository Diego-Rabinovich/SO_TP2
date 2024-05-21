#include "include/scheduler.h"
#include "include/processList.h"

int CheckProcessIntegrity(PCB * process_pcb);


int nextId = 1;
QueueHeader * queues[4];

void AddProcess(PCB * process_pcb) {
    if (!CheckProcessIntegrity(process_pcb)){
        return;
    }
    PushProcess(queues[process_pcb->priority], process_pcb);
}

void  DeleteProcess(PCB * process_pcb){
    if (!CheckProcessIntegrity(process_pcb)){
        return;
    }
    RemoveProcess(queues[process_pcb->priority], process_pcb->id );
}

int GetFirstAvailableQueue(){
    for (int i = 3 ; i >= 0; i--){
        if(queues[i]->count > 0)
            return i;
    }
    return -1;
}


/**
 * Priority-Based-Round-Robin scheduler
 * @return the PCB of the next process or NULL if there is no process waiting or something went wrong
 */
PCB * GetNextProcess(){
    int queue_idx = GetFirstAvailableQueue();
    if (queue_idx < 0){
        return NULL;
    }
    PCB * pcb = PopProcess(queues[queue_idx]);

    while(pcb->p_state == TERMINATED){
        DeleteProcess(pcb);
        queue_idx = GetFirstAvailableQueue());
        if(queue_idx < 0){
            return NULL
        }
        pcb = PopProcess(queues[queue_idx]);
    }

    return pcb;
}


void Init(){
 //priority queues
    queues[0] = malloc(sizeof QueueHeader);
    queues[1] = malloc(sizeof QueueHeader);
    queues[2] = malloc(sizeof QueueHeader);
    queues[3] = malloc(sizeof QueueHeader);

}

int CheckProcessIntegrity(PCB * process_pcb){
    if (process_pcb == NULL || process_pcb->priority > 3 || process_pcb->priority < 0){
        return 0;
    }
    return 1;
}