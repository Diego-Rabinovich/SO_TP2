// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/process.h"

#include "fileDescriptor.h"
#include "include/interrupts.h"
#include "include/lib.h"
#include "include/memoryManager.h"
#include "include/scheduler.h"
#define P_STACK_SIZE 8192

void runProcess(Main main_func, char** args, int argc) {
    int32_t ret = main_func(argc, args);
    killCurrent(ret);
}

void initializeProcess(PCB* process_pcb, uint16_t pid, uint16_t parent_pid,
                       Main main_func, char** args, char* name,
                       uint8_t priority, int16_t fds[]) {
    process_pcb->pid = pid;
    process_pcb->parent_pid = parent_pid;
    process_pcb->priority = priority;
    int name_len = strLen(name) + 1;
    process_pcb->name = memAlloc(name_len + 1);
    memcpy(process_pcb->name, name, name_len + 1);
    process_pcb->name[name_len - 1] = 0;
    process_pcb->waiting_pid = 0;
    process_pcb->p_state = READY;
    process_pcb->ret = NULL;

    for (int i = 0; i < MAX_PROCESSES; ++i) {
        process_pcb->children[i] = 0;
    }
    process_pcb->childrenCount = 0;

    for (int i = 0; i < 3; i++) {
        process_pcb->fds[i] = fds[i];
    }

    if (fds[STDIN] == STDIN) {
        process_pcb->isFg = 1; // if p interacts with user stdin then is fg.
    }
    else {
        process_pcb->isFg = 0;
    }
    if(fds[STDIN] != DEV_NULL) {
        changeFdReader(fds[STDIN], pid);
    }
    if(fds[STDOUT] != STDOUT && fds[STDOUT] != DEV_NULL) {
        changeFdWriter(fds[STDOUT], pid);
    }

    // CONFIGURAMOS LOS ARGUMENTOS
    int argc = arrayLen((void*)args);
    int total_args_len = 0;
    int args_len[argc];

    for (int i = 0; i < argc; i++) {
        args_len[i] = strLen(args[i]) + 1;
        total_args_len += args_len[i];
    }

    int size = sizeof(char**) * (argc + 1);
    char** args_array = (char**)memAlloc(total_args_len + size);
    char* start_ptr = (char*)args_array + size;

    for (int i = 0; i < argc; i++) {
        args_array[i] = start_ptr;
        memcpy(start_ptr, args[i], args_len[i]);
        start_ptr += args_len[i];
    }

    process_pcb->argv = args_array;
    process_pcb->argv[argc] = NULL;

    process_pcb->rsb = memAlloc(P_STACK_SIZE);
    process_pcb->rsp = create_sf(runProcess, main_func,
                                 (void*)((uint64_t)process_pcb->rsb + P_STACK_SIZE),
                                 (void*)process_pcb->argv, argc);
}

void freeProcess(PCB* process_pcb) {
    memFree(process_pcb->name);
    memFree(process_pcb->rsb);
    memFree(process_pcb->argv);
    memFree(process_pcb);
}

int isWaiting(PCB* pcb, int16_t pid_to_wait) {
    return pcb->waiting_pid == pid_to_wait;
}

ProcessInfo* loadInfo(ProcessInfo* info, PCB* pcb) {
    info->pid = pcb->pid;
    info->parent_pid = pcb->parent_pid;
    info->piority = pcb->priority;
    info->name = memAlloc(strLen(pcb->name) + 1);
    memcpy(info->name, pcb->name, strLen(pcb->name));
    info->p_state = pcb->p_state;
    info->rsb = pcb->rsb;
    info->rsp = pcb->rsp;
    info->fg = pcb->isFg;
    return info;
}
