#ifndef TPEARQUI_USERLIBASM_H
#define TPEARQUI_USERLIBASM_H

#include <stdint.h>
typedef int (*Main)(int argc, char **args);

typedef struct {
    uint64_t x;
    uint64_t y;
}Coords;

void sys_paintPixel(uint64_t x, uint64_t y, uint32_t hexColor);
void sys_getScreenDimensions(Coords* ret);
void sys_getCpuTime(int ret[3]);
void waitInt();
void sys_resetCursor();
void sys_blankArea(uint64_t x0, uint64_t xf, uint64_t y0, uint64_t yf, uint32_t hexColor);
void sys_yield();
void sys_free( void *  ptr);
void sys_memset(void * dest, char character, uint64_t length);
void sys_beep(uint32_t frequency, uint32_t millis);
void make_invalid_opCode();
void* sys_malloc(unsigned long bytes);
int sys_write(uint64_t fd, char * text, uint64_t len, uint32_t fontHexColor, uint32_t bgHexColor,  uint32_t fontSize);
int sys_read(uint64_t fd,char *buf,uint64_t count);
int sys_hardRead(uint64_t fd,char * buf,uint64_t count);
int sys_waitNMillis(uint64_t millis);
int16_t sys_createProcess(Main main_func, char** args, char* name, uint8_t priority, int16_t fds[]);
int32_t sys_kill(uint16_t pid);
int32_t sys_block(uint16_t pid);
int32_t sys_unblock(uint16_t pid);
int32_t sys_nice(uint16_t pid, uint8_t new_prio);
int32_t sys_pid();
void sys_ps();
int32_t sys_sem_new(char * name, uint32_t initial_value);
void sys_sem_destroy(char * name);
int8_t sys_sem_wait(char * name);
int8_t sys_sem_post(char * name);
uint64_t sys_wait_pid(int16_t pid);
void sys_get_FDs(int16_t target[3]);
int16_t sys_openFd();
void sys_closeFd(int16_t fd);
#endif //TPEARQUI_USERLIBASM_H
