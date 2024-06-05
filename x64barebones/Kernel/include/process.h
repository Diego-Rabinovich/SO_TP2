#ifndef SO_TP2_PROCESSLIST_H
#define SO_TP2_PROCESSLIST_H

#include "infoStructs.h"
#include "lib.h"

#define DEFAULT_FDS 3

typedef struct PCB {
  uint16_t pid;
  uint16_t parent_pid;
  uint16_t waiting_pid;

  void *rsb;
  void *rsp;

  char **argv;
  char *name;

  uint8_t priority;
  PState p_state;

  int16_t fds[DEFAULT_FDS];
  int32_t ret;
} PCB;

void initializeProcess(PCB *process, uint16_t pid, uint16_t parent_pid,
                       Main main_func, char **args, char *name,
                       uint8_t priority); //, int16_t fds[]

void freeProcess(PCB *pcb);

int isWaiting(PCB *pcb, uint16_t pid_to_wait);

ProcessInfo *loadInfo(ProcessInfo *info, PCB *pcb);

#endif // SO_TP2_PROCESSLIST_H
