#include "include/scheduler.h"
#include "include/process.h"
#include "include/memoryManager.h"
#include "include/linkedList.h"
#include "include/lib.h"
#include "include/videoDriver.h"
#define TRIVIAL_PID 0
#define QTY_PRIORITIES 4
#define MAX_PROCESSES 4096

typedef struct Scheluler
{
    Node *processes[MAX_PROCESSES];
    LinkedList ready_processes;
	uint16_t running_pid;
	uint16_t next_pid;
	uint16_t process_count;
    uint16_t fg_pid;
	uint8_t remaining_rounds;
} Scheduler;

Scheduler scheduler;

void schedulerInit(){
	for (int i = 0; i < MAX_PROCESSES; i++) {
        scheduler.processes[i] = NULL;
    }
    scheduler.ready_processes = createLinkedList();
    scheduler.next_pid = 0;
    scheduler.fg_pid = 0;
}

/**
 * Sets next_pid to the first null of the process array
 */
static void setNextPID(){
    while (scheduler.processes[scheduler.next_pid] != NULL){
        scheduler.next_pid = (scheduler.next_pid + 1) % MAX_PROCESSES;
    }
}

static uint16_t getNextReadyProcess(){
    PCB *pcb = NULL;
    pcb = (PCB *)((Node *) listNext(scheduler.ready_processes))->data;
	if (pcb == NULL) {
        return TRIVIAL_PID;
    }
    return pcb->pid;
}

int32_t setPriority(uint16_t pid, uint8_t new_prio){
    Node *node = scheduler.processes[pid];
	if (node == NULL || pid == TRIVIAL_PID || new_prio >= QTY_PRIORITIES) {
        return -1;
    }
    PCB *pcb = (PCB *) node->data;
	pcb->priority = new_prio;
	return new_prio;
}

uint8_t setState(uint16_t pid, uint8_t new_state) {
    Node *node = scheduler.processes[pid];
    if (node == NULL || pid == TRIVIAL_PID || new_state == RUNNING){
        return -1;
    }
    PCB *pcb = (PCB *) node->data;
	PState old_state = pcb->p_state;
	if (new_state == pcb->p_state){
		return new_state;
    }
	pcb->p_state = new_state;
	if (new_state == BLOCKED) {
		remove(scheduler.ready_processes, node);
        if (pid == scheduler.running_pid){
            yield();
        }
	}
	else if (new_state == READY) {
		push(scheduler.ready_processes, node);
	}
	return new_state;
}

PState getPState(uint16_t pid){
    Node *process_node = scheduler.processes[pid];
	if (process_node == NULL) {
        return TERMINATED;
    }
	return ((PCB *) process_node->data)->p_state;
}

void* schedule(void* last_rsp) {
    scheduler.remaining_rounds--;
    Node * running = scheduler.processes[scheduler.running_pid];
    if (scheduler.remaining_rounds > 0 && running!=NULL){
        return last_rsp;
    }
    if(((PCB *)running->data)->p_state == RUNNING){
        ((PCB *)running->data)->priority--;
        ((PCB *)running->data)->p_state = READY;
    }
    uint16_t new_PID = getNextReadyProcess();
    scheduler.remaining_rounds = ((PCB *) scheduler.processes[new_PID]->data)->priority + 1;
    return ((PCB *)scheduler.processes[new_PID]->data)->rsp;
}

uint16_t createProcess(Main main_func, char **args, char *name,
    uint8_t priority) { //int16_t fds[]
    if (scheduler.process_count >= MAX_PROCESSES || main_func == NULL || priority < 0 || priority > 3 ){ // fds == NULL
        return -1;
    }
	PCB *pcb = (PCB *) memAlloc(sizeof(PCB));
    initializeProcess(pcb, scheduler.next_pid, scheduler.running_pid, main_func, args, name, priority); //fds

	Node *process_node;
    process_node = memAlloc(sizeof(Node));
    process_node->data = (void *) pcb;

    if (pcb->pid != TRIVIAL_PID){
        queue(scheduler.ready_processes,  process_node);
    }

	scheduler.processes[pcb->pid] = process_node;
	scheduler.process_count++;
    setNextPID();
	return pcb->pid;
}

int32_t killCurrent(int32_t ret){
    return kill(scheduler.running_pid, ret);
}

int32_t kill(uint16_t pid, int32_t ret){
    Node *to_kill_node = scheduler.processes[pid];
	if (to_kill_node == NULL || pid == TRIVIAL_PID) {
        return -1;
    }
	PCB *to_kill_pcb = (PCB *) to_kill_node->data;

    //TODO implementar con IPC
	//closeFDs(to_kill_pcb);

    if (to_kill_pcb->p_state != BLOCKED ){
	    remove(scheduler.ready_processes, to_kill_node);
    }

	to_kill_pcb->ret = ret;

	to_kill_pcb->p_state = TERMINATED;

	Node *parent_node = scheduler.processes[to_kill_pcb->parent_pid];
	if (parent_node != NULL) {
		PCB *parent_pcb = (PCB *) parent_node->data;
		if (isWaiting(parent_pcb, to_kill_pcb->pid)) {
            setState(to_kill_pcb->parent_pid, READY);
        }
	}

	if (pid == scheduler.running_pid){
		yield();
    }
	return 0;
}

uint16_t getPid(){
    return scheduler.running_pid;
}

ProcessInfoArray *getProcessArray(){
    ProcessInfoArray *info_array = memAlloc(sizeof(ProcessInfoArray));
	ProcessInfo *array = memAlloc(scheduler.process_count * sizeof(ProcessInfo));

    for ( int i = 0, process_idx = 0; process_idx < scheduler.process_count && i < MAX_PROCESSES; i++){
        Node *current = (Node *) scheduler.processes[i];
        if (current != NULL){ //processes is ready, running or blocked
            loadInfo(&array[process_idx], (PCB *) current->data);
            process_idx++;
        }
    }

	info_array->length = scheduler.process_count;
	info_array->array = array;
	return info_array;
}

void yield(){
    scheduler.remaining_rounds = 0;
	callTimerTick();
}

//TODO implementar con IPC

void killFG(){
    kill(scheduler.fg_pid, -1);
}
