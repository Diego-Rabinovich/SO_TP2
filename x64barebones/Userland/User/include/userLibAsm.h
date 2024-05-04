#ifndef TPEARQUI_USERLIBASM_H
#define TPEARQUI_USERLIBASM_H

#include <stdint.h>
typedef struct {
    uint64_t x;
    uint64_t y;
}Coords;

int sys_read(uint64_t fd,char *buf,uint64_t count);
int sys_write(uint64_t fd, char * text, uint64_t len, uint32_t fontHexColor, uint32_t fontSize);
void sys_paintPixel(uint64_t x, uint64_t y, uint32_t hexColor);
void sys_getScreenDimensions(Coords* ret);
void sys_getCpuTime(int ret[3]);
void waitInt();
void sys_resetCursor();
void sys_blankArea(uint64_t x0, uint64_t xf, uint64_t y0, uint64_t yf, uint32_t hexColor);
int sys_hardRead(uint64_t fd,char * buf,uint64_t count);
int sys_waitNMillis(uint64_t millis);
void sys_beep(uint32_t frequency, uint32_t millis);
void* sys_malloc(unsigned long bytes);
void* sys_calloc(unsigned long  units, unsigned long  unitSize);
void make_invalid_opCode();
#endif //TPEARQUI_USERLIBASM_H
