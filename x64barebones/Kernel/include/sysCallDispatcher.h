#ifndef TPE_SYSCALLDISPATCHER_H
#define TPE_SYSCALLDISPATCHER_H
#include "stdint.h"

typedef struct {
    uint64_t x;
    uint64_t y;
} coords;

uint64_t sysCallDispatcher(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
                       uint64_t arg4, uint64_t arg5, uint64_t RAX);
void ps();
#endif //TPE_SYSCALLDISPATCHER_H
