#ifndef SO_TP2_SCHEDULER_H
#define SO_TP2_SCHEDULER_H

#include "lib.h"
#include "process.h"

void schedulerInit();
uint16_t createProcess(Main main_func, char **args, char *name, uint8_t priority, int16_t fds[]);
int32_t kill(uint16_t pid, int32_t ret);
int32_t killCurrent(int32_t ret);
uint16_t getPid();
PState getPState(uint16_t pid);
int32_t setPriority(uint16_t pid, uint8_t new_prio);
uint8_t setState(uint16_t pid, uint8_t new_state);
ProcessInfoArray *getProcessArray();
void * schedule(void* last_rsp);
void yield();
int8_t changeFD(uint16_t pid, uint8_t fd_idx, int16_t new_fd);
void killFG();
uint64_t waitPid(int16_t pid);
void getFDs(int16_t target[3]);
#endif // SO_TP2_SCHEDULER_H
