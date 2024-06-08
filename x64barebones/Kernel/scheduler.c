#include "include/scheduler.h"
#include "include/process.h"
#include "include/memoryManager.h"
#include "include/linkedList.h"
#include "include/lib.h"
#include "include/interrupts.h"
#define TRIVIAL_PID 0
#define INIT 1
#define QTY_PRIORITIES 4
#define MAX_PROCESSES 128

typedef struct Scheluler
{
    Node *processes[MAX_PROCESSES];
    LinkedList ready_processes;
	uint16_t running_pid;
	uint16_t next_pid;
	uint16_t process_count;
    uint16_t fg_pid;
	int8_t remaining_rounds;
} Scheduler;

Scheduler scheduler;
char is_creating = 0;

void schedulerInit(){
	for (int i = 0; i < MAX_PROCESSES; i++) {
        scheduler.processes[i] = NULL;
    }
    scheduler.ready_processes = createLinkedList();
    scheduler.next_pid = 0;
    scheduler.fg_pid = 0;
    scheduler.remaining_rounds = 0;
    scheduler.running_pid = 0;
}

/**
 * Sets next_pid to the first null of the process array
 */
static void setNextPID(){
    while (scheduler.processes[scheduler.next_pid] != NULL){
        scheduler.next_pid = scheduler.next_pid + 1;
        if(scheduler.next_pid == MAX_PROCESSES) {
            scheduler.next_pid = 0;
        }
    }
}

static uint16_t getNextReadyProcess(){
    startIterator(scheduler.ready_processes);
    Node * node =  ((Node *) listNext(scheduler.ready_processes));
	if (node == NULL) {
        return TRIVIAL_PID;
    }
    return ((PCB *)node->data)->pid;
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
    if(is_creating) {
        return last_rsp;
    }
    static int first_round = 1;
    Node * running = scheduler.processes[scheduler.running_pid];
    if (scheduler.remaining_rounds > 0 && running!=NULL){
        scheduler.remaining_rounds--;
        return last_rsp;
    }
    if( running!=NULL && ((PCB *)running->data)->p_state == RUNNING && scheduler.running_pid != TRIVIAL_PID){
        if(((PCB *)running->data)->priority > 0){
            ((PCB *)running->data)->priority--;
        }
        ((PCB *)running->data)->p_state = READY;
        remove(scheduler.ready_processes, running);
        queue(scheduler.ready_processes, running);
    } else if (running != NULL && scheduler.running_pid == TRIVIAL_PID){
        ((PCB *)running->data)->p_state = READY;
    }

    uint16_t new_PID = getNextReadyProcess();
    PCB * new_P = scheduler.processes[new_PID]->data;
    scheduler.remaining_rounds = new_P->priority + 1;
    if(!first_round && running!=NULL){
        ((PCB *)running->data)->rsp = last_rsp;
    } else{
        first_round = 0;
    }
//    if(running!=NULL && ((PCB *)running->data)->p_state==TERMINATED){
//        scheduler.processes[((PCB *)running->data)->pid]=NULL;
//        freeProcess(running->data);
//        memFree(running);
//        scheduler.process_count--;
//    }
    new_P->p_state = RUNNING;
    scheduler.running_pid = new_PID;
    return new_P->rsp;
}

uint16_t createProcess(Main main_func, char **args, char *name, uint8_t priority, int16_t fds[]) {
    is_creating = 1;
    if (scheduler.process_count >= MAX_PROCESSES || main_func == NULL || priority < 0 || priority > 3 || fds == NULL){
        is_creating =0;
        return -1;
    }
	PCB *pcb = (PCB *) memAlloc(sizeof(PCB));

    if(scheduler.next_pid != TRIVIAL_PID && fds[0] == STDIN) {
        PCB * parent = (PCB *)scheduler.processes[scheduler.running_pid]->data;
        if(parent->isFg){
            parent->fds[0] = DEV_NULL;
            parent->isFg = 0;
            scheduler.fg_pid = scheduler.next_pid;
        }
        else{
            is_creating = 0;
            return -1;
        }
    }

    initializeProcess(pcb, scheduler.next_pid, scheduler.running_pid, main_func, args, name, priority, fds);


	Node *process_node;
    process_node = memAlloc(sizeof(Node));
    process_node->data = (void *) pcb;


    if (pcb->pid != TRIVIAL_PID){
        queue(scheduler.ready_processes,  process_node);
        pcb->p_state = READY;
        PCB * parent_pcb = (PCB *)scheduler.processes[scheduler.running_pid]->data;
        parent_pcb->children[parent_pcb->childrenCount++] = scheduler.next_pid;
    }

	scheduler.processes[pcb->pid] = process_node;
	scheduler.process_count++;
    setNextPID();
    is_creating = 0;
	return pcb->pid;
}

int32_t killCurrent(int32_t ret){
    return kill(scheduler.running_pid, ret);
}

int32_t kill(uint16_t pid, int32_t ret){
    Node *to_kill_node = scheduler.processes[pid];
	if (to_kill_node == NULL || pid == TRIVIAL_PID || pid <= 2) {
        //note that if pid <= 2 then p is either sh, user, or trivial
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
		if (isWaiting(parent_pcb, to_kill_pcb->pid)|| isWaiting(parent_pcb,-1)) {
            setState(to_kill_pcb->parent_pid, READY);
            ((PCB*)parent_node->data)->ret=ret;
        }
        if(to_kill_pcb->isFg) {
            scheduler.fg_pid = to_kill_pcb->parent_pid;
            parent_pcb->isFg = 1;
            parent_pcb->fds[0] = STDIN;
        }
	}

    for (int i = 0; i < to_kill_pcb->childrenCount; ++i) {
        uint16_t child_pid = to_kill_pcb->children[i];
        if(scheduler.processes[child_pid] != NULL) { //if child not DEAD
            ((PCB *)scheduler.processes[child_pid]->data)->parent_pid = to_kill_pcb->parent_pid;
        }
    }

    freeProcess(to_kill_pcb);
    memFree(to_kill_node);
    scheduler.process_count--;
    scheduler.processes[pid]=NULL;


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
    _sti();
	callTimerTick();
}

//TODO implementar con IPC

void killFG(){
    if(scheduler.fg_pid > 2){
        kill(scheduler.fg_pid, -1);
    }
}

uint64_t waitPid(int16_t pid){
    ((PCB *)scheduler.processes[scheduler.running_pid]->data)->waiting_pid=pid;
    setState(scheduler.running_pid,BLOCKED);
    return ((PCB*)scheduler.processes[scheduler.running_pid]->data)->ret;
}

void getFDs(int16_t target[3]){
    int16_t* fds = ((PCB*)scheduler.processes[scheduler.running_pid]->data)->fds;
    for (int i = 0; i < 3; ++i) {
        target[i]=fds[i];
    }
}