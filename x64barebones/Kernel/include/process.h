#ifndef SO_TP2_PROCESSLIST_H
#define SO_TP2_PROCESSLIST_H

#include "infoStructs.h"
#include "lib.h"
#define MAX_PROCESSES 128

#define DEFAULT_FDS 3
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define DEV_NULL 9

typedef struct PCB {
    uint16_t pid;
    uint16_t parent_pid;
    int16_t waiting_pid;

    void *rsb;
    void *rsp;

    char **argv;
    char *name;

    uint8_t priority;
    PState p_state;
    uint8_t isFg;
    int16_t fds[DEFAULT_FDS];   //Los FDs pueden ser reemplazados por pipes pero no se agregan nuevos FDs
    int32_t ret;
    uint16_t children[MAX_PROCESSES];
    unsigned char childrenCount;

} PCB;

void initializeProcess(PCB *process, uint16_t pid, uint16_t parent_pid,
                       Main main_func, char **args, char *name,
                       uint8_t priority, int16_t fds[]);

void freeProcess(PCB *pcb);

int isWaiting(PCB *pcb, int16_t pid_to_wait);

ProcessInfo *loadInfo(ProcessInfo *info, PCB *pcb);

#endif // SO_TP2_PROCESSLIST_H
