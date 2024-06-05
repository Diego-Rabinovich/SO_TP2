#include "include/sysCallDispatcher.h"
#include <stdint.h>
#include "include/console.h"
#include "include/keyboardDriver.h"
#include "include/videoDriver.h"
#include "include/time.h"
#include "include/lib.h"
#include "include/audioDriver.h"
#include "include/interrupts.h"
#include "include/memoryManager.h"

int read(uint64_t fd, char *buf, uint64_t count);

int write(uint64_t fd, char *text, uint64_t len, uint32_t fontHexColor, uint32_t fontSize);

void printPixel(uint64_t x, uint64_t y, uint32_t color);
int waitNMillis(uint64_t millis);

void getScreenDimensions(coords *ret);
uint64_t * dumpRegs();
char stored();
int hardRead(uint64_t fd, char *buf, uint64_t count);
void processBuf(char * buf,int *idx);

uint64_t sysCallDispatcher(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
                      uint64_t arg4, uint64_t arg5, uint64_t RAX) {

    switch (RAX) {
        case 0:
            return read(arg0, (char *) arg1, arg2);
        case 1:
            return write(arg0, (char *) arg1, arg2, arg3, arg4);
        case 2:
            printPixel(arg0, arg1, (uint32_t) arg2);
            return RAX;
        case 3:
            getScreenDimensions((coords *) arg0);
            return RAX;
        case 4:
            getCurrentCpuTime((int*)arg0);
            return RAX;
        case 5:
            resetCursor();
            return RAX;
        case 6:
            blankArea(arg0+X_START_OFFSET, arg1, arg2+Y_START_OFFSET, arg3, arg4);
            return RAX;
        case 7:
            return waitNMillis(arg0);
        case 8:
            return hardRead(arg0, (char *) arg1, arg2);
        case 9:
            beep(arg0,arg1);
            return RAX;
        case 10:
            return (uint64_t) memAlloc(arg0);
        case 11:
            memFree((void *) arg0);
            return RAX;
        case 12:
            memset((void *) arg0, (char) arg1, arg2);
            return RAX;
        default:
            return -1;
    }
}

int read(uint64_t fd, char *buf, uint64_t count) {
    switch (fd) {
        case 0: {
            int idx = 0;
            alignPointers();
            while (idx < count) {
                _hlt();
                if(hasNext()){
                    processBuf(buf,&idx);
                }
            }
            return idx;
        }
        default:
            return -1;
    }

}


void processBuf(char * buf,int *idx){
    unsigned char code = next();
    if (code == E_KEYS) {
        code = next();
        char flag = 0;
        if (code & RELEASE) {
            flag = 1;
            code -= RELEASE;
        }
        switch (code) {
            case GREY_UP_SCAN:
                code = UP_ARROW;
                break;
            case GREY_LEFT_SCAN:
                code = LEFT_ARROW;
                break;
            case GREY_RIGHT_SCAN:
                code = RIGHT_ARROW;
                break;
            case GREY_DOWN_SCAN:
                code = DOWN_ARROW;
                break;
            case (R_CTRL_SCAN | R_ALT_SCAN):
                code = getStringFromCode(code);
                break;
            default:
                code = 0;
                break;
        }
        if (code) {
            if (flag) {
                code += RELEASE;
            }
            buf[(*idx)++] = code;
        }
    } else {
        char flag = 0;
        if (code & RELEASE) {
            flag = 1;
        }
        char c = getStringFromCode(code);

        if (c != NOT_USABLE) {
            if (flag) {
                c += RELEASE;
            }
            buf[(*idx)++] = c;
        }
    }
}
int hardRead(uint64_t fd, char *buf, uint64_t count){
    switch (fd) {
        case 1:{
            int idx = 0;
            while (idx < count && hasNext()) {
                processBuf(buf, &idx);
            }
            alignPointers();
            return idx;
        }
    }
    return 0;
}

int write(uint64_t fd, char *text, uint64_t len, uint32_t font_hexColor, uint32_t fontSize) {
    switch (fd) {
        case 1:
            drawStringWithColor(text,len,font_hexColor,0x00000000, fontSize);
            break;
        case 2:
            drawStringWithColor(text,len,0x00FF0000,0x00000000, fontSize); //rojo y negro respectivamente
            break;
    }
    return 0;
}

void printPixel(uint64_t x, uint64_t y, uint32_t color) {
    putPixel(color, x + X_START_OFFSET, y + Y_START_OFFSET);
}

void getScreenDimensions(coords *ret) {
    ret->x = X_LIMIT-X_START_OFFSET;
    ret->y = Y_LIMIT-Y_START_OFFSET+8;
}