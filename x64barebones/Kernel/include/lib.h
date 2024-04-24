#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include "time.h"

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
char *cpuVendor(char *result);
int* cpuGetTime();
void getCurrentCpuTime(int array[3]);
void waitForInt();
void infoRegs();
int waitNMillis(uint64_t millis);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
int strLen(const char * s);

#endif
