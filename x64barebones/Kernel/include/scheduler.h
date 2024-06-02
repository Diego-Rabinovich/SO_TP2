#ifndef SO_TP2_SCHEDULER_H
#define SO_TP2_SCHEDULER_H

#include "include/process.h"
#include "include/lib.h"
#include "include/infoStructs.h"

void schedulerInit();
int16_t createProcess(Main main_func, char **args, char *name,
    uint8_t priority, int16_t fds[], uint8_t is_unkillable);
int32_t kill(uint16_t pid, int32_t ret);
int32_t killCurrent(int32_t ret);
uint16_t getPid();
PState getPState(uint16_t pid);
int32_t setPriority(uint16_t pid, uint8_t new_prio);
int8_t setState(uint16_t pid, uint8_t new_state);
ProcessInfoArray *getProcessArray();
int32_t isAlive(uint16_t pid);
void yield();
int8_t changeFD(uint16_t pid, uint8_t fd_idx, int16_t new_fd);
int16_t getFD(uint8_t fd_idx);
void killFG();

#endif //SO_TP2_SCHEDULER_H
