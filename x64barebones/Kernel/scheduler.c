#include "include/scheduler.h"
#include "include/process.h"
#include "include/memoryManager.h"
#include "include/linkedList.h"

#define IDLE_PID 0
#define QTY_PRIORITIES 4
#define MAX_PROCESSES (1 << 12)

typedef struct Scheluler
{
    Node *processes[MAX_PROCESSES];
    LinkedList process_lists[QTY_PRIORITIES + 1];
	uint16_t running_pid;
	uint16_t next_pid;
	uint16_t process_count;
	int8_t quantum_left;
	int8_t kill_fg_flag;
} Scheduler;

Scheduler scheduler;

void schedulerInit(){
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler.processes[i] = NULL;
	for (int i = 0; i < QTY_PRIORITIES + 1; i++)
		scheduler.process_lists[i] = createLinkedListADT();
	scheduler.next_pid = 0;
	scheduler.kill_fg_flag = 0;
}

static uint16_t getNextPID(){
    PCB *pcb = NULL;
	for (int prio = QTY_PRIORITIES - 1; prio >= 0 && pcb == NULL; prio--)
		if (!isEmpty(scheduler.process_lists[prio]))
			pcb = (PCB *) (pop(scheduler.process_lists[prio]))->data;

	if (pcb == NULL)
		return IDLE_PID;
	return pcb->pid;
}

int32_t setPriority(uint16_t pid, uint8_t new_prio){
    Node *node = scheduler.processes[pid];
	if (node == NULL || pid == IDLE_PID)
		return -1;
	PCB *pcb = (PCB *) node->data;

	if (new_prio >= QTY_PRIORITIES)
		return -1;
	if (pcb->p_state == READY || pcb->p_state == RUNNING) {
		remove(scheduler.process_lists[pcb->priority], node);
		scheduler.processes[pcb->pid] = queue(scheduler.process_lists[new_prio], node);
	}
	pcb->priority = new_prio;
	return new_prio;
}

int8_t setState(uint16_t pid, uint8_t new_state){
    Node *node = scheduler.processes[pid];
	if (node == NULL || pid == IDLE_PID)
		return -1;
	PCB *pcb = (PCB *) node->data;
	PState old_state = pcb->p_state;
	if (new_state == RUNNING)
		return -1;
	if (new_state == pcb->p_state)
		return new_state;

	pcb->p_state = new_state;
	if (new_state == BLOCKED) {
		remove(scheduler.process_lists[pcb->priority], node);
		queue(scheduler.process_lists[QTY_PRIORITIES], node);
	}
	else if (old_state == BLOCKED) {
		remove(scheduler.process_lists[QTY_PRIORITIES], node);
		push(scheduler.process_lists[pcb->priority], node);
	}
	return new_state;
}

PState getPState(uint16_t pid){
    Node *processNode = scheduler.processes[pid];
	if (processNode == NULL)
		return TERMINATED;
	return ((PCB *) processNode->data)->p_state;
}

void* schedule(void* last_rsp){
    static int first = 1;

    scheduler.quantum_left--;
	if (!scheduler.process_count || scheduler.quantum_left > 0)
		return last_rsp;

	PCB *current_process_pcb;
	Node *current_process_node = scheduler.processes[scheduler.running_pid];

	if (current_process_node != NULL) {
		current_process_pcb = (PCB *) current_process_node->data;
		if (!first)
			current_process_pcb->rsp = last_rsp;
		else
			first = 0;
		if (current_process_pcb->p_state == RUNNING)
			current_process_pcb->p_state = READY;

		uint8_t new_prio = current_process_pcb->priority > 0 ? current_process_pcb->priority - 1 : current_process_pcb->priority;
		setPriority(current_process_pcb->pid, new_prio);
	}

	scheduler.running_pid = getNextPID();
	current_process_pcb = scheduler.processes[scheduler.running_pid]->data;

	if (scheduler.kill_fg_flag && current_process_pcb->fds[STDIN] == STDIN) {
		scheduler.kill_fg_flag = 0;
		if (killCurrent(-1) != -1)
			callTimerTick();
	}
	scheduler.quantum_left = (QTY_PRIORITIES - 1 - current_process_pcb->priority);
	current_process_pcb->p_state = RUNNING;
	return current_process_pcb->rsp;
}

int16_t createProcess(Main main_func, char **args, char *name,
    uint8_t priority, int16_t fds[], uint8_t is_unkillable){
    if (scheduler.process_count >= MAX_PROCESSES)
        return -1;
	PCB *pcb = (PCB *) memAlloc(sizeof(PCB));
	initializeProcess(pcb, scheduler.next_pid, scheduler.running_pid, main_func, args, name, priority, fds, is_unkillable);

	Node *process_node;
	if (pcb->pid != IDLE_PID)
		process_node = queue(scheduler.process_lists[pcb->priority], (void *) pcb);
	else {
		process_node = memAlloc(sizeof(Node));
		process_node->data = (void *) pcb;
	}
	scheduler.processes[pcb->pid] = process_node;

	while (scheduler.processes[scheduler.next_pid] != NULL)
		scheduler.next_pid = (scheduler.next_pid + 1) % MAX_PROCESSES;
	scheduler.process_count++;
	return pcb->pid;
}

int32_t killCurrent(int32_t ret){
    return killProcess(scheduler.running_pid, ret);
}

int32_t kill(uint16_t pid, int32_t ret){
    Node *to_kill_node = scheduler.processes[pid];
	if (to_kill_node == NULL)
		return -1;
	PCB *to_kill_pcb = (PCB *) to_kill_node->data;
	if (to_kill_pcb->is_unkillable)
		return -1;

	closeFDs(to_kill_pcb);

	uint8_t priority_idx = to_kill_pcb->p_state != BLOCKED ? to_kill_pcb->priority : QTY_PRIORITIES;
	remove(scheduler.process_lists[priority_idx], to_kill_node);
	to_kill_pcb->ret = ret;

	to_kill_pcb->p_state = TERMINATED;

	Node *parent_node = scheduler.processes[to_kill_pcb->parent_pid];
	if (parent_node != NULL) {
		PCB *parent_pcb = (PCB *) parent_node->data;
		if (is_waiting(parent_pcb, to_kill_pcb->pid))
			setState(to_kill_pcb->parent_pid, READY);
	}
	if (pid == scheduler.running_pid)
		yield();
	return 0;
}

uint16_t getPid(){
    return scheduler.running_pid;
}

ProcessInfoArray *getProcessArray(){
    ProcessInfoArray *info_array = allocMemory(sizeof(ProcessInfoArray));
	ProcessInfo *array = allocMemory(scheduler.process_count * sizeof(ProcessInfo));
	int process_idx = 0;

	loadInfo(&array[process_idx++], (PCB *) scheduler.processes[IDLE_PID]->data);

	for (int prio = QTY_PRIORITIES; prio >= 0; prio--) {
		startIterator(scheduler.process_lists[prio]);
		while (hasNext(scheduler.process_lists[prio])) {
			PCB *current = (PCB *) next(scheduler.process_lists[prio]);
			loadInfo(&array[process_idx], current);
			process_idx++;
		}
	}
	info_array->length = scheduler.process_count;
	info_array->array = array;
	return info_array;
}

int32_t isAlive(uint16_t pid){
    Node *node = scheduler.processes[pid];
	return node != NULL;
}

void yield(){
    scheduler.quantum_left = 0;
	callTimerTick();
}

int8_t changeFD(uint16_t pid, uint8_t fd_idx, int16_t new_fd){
    Node *processNode = scheduler.processes[pid];
	if (pid == IDLE_PID || processNode == NULL)
		return -1;
	PCB *process = (PCB *) processNode->data;
	process->fds[fd_idx] = new_fd;
	return 0;
}

int16_t getFD(uint8_t fd_idx){
    PCB *process = scheduler.processes[scheduler.running_pid]->data;
	return process->fds[fd_idx];
}

void killFG(){
    scheduler.kill_fg_flag = 1;
}
