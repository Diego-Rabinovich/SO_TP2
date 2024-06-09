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
#include "include/scheduler.h"
#include "include/fileDescriptor.h"
#include "include/semaphores.h"


void printPixel(uint64_t x, uint64_t y, uint32_t color);
int waitNMillis(uint64_t millis);

void getScreenDimensions(coords *ret);
uint64_t * dumpRegs();
char stored();
int hardRead(uint64_t fd, char *buf, uint64_t count);
void processBuf(char * buf,int *idx);
void ps();

uint64_t sysCallDispatcher(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
                      uint64_t arg4, uint64_t arg5, uint64_t RAX) {

    switch (RAX) {
        case 0:
            return readOnFile(getFd(arg0), (unsigned char *) arg1, arg2);
        case 1:
            return writeOnFile(getFd(arg0), (char *) arg1, arg2, arg3, arg4, arg5);
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

        //TODO: afuera
        case 8:
            return hardRead(arg0, (char *) arg1, arg2);
        //TODO: afuera
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
        case 13:
            return createProcess((Main) arg0, (char**) arg1, (char*) arg2, (uint8_t) arg3, (int16_t*)arg4);
        case 14:
            return kill(arg0, -1);
        case 15:
            return setState(arg0, BLOCKED);
        case 16:
            return setState(arg0, READY);
        case 17:
            return setPriority(arg0, arg1);
        case 18:
            return getPid();
        case 19:
            yield();
            return RAX;
        case 20:
            return waitPid(arg0);
        case 21:
             ps();
             return RAX;
        case 22:
            getFDs((int16_t *)arg0);
            return RAX;
        case 23:
            return newSemaphore((char *) arg0, arg1);
        case 24:
            semDestroy((char *) arg0);
            return RAX;
        case 25:
            return semPost((char *) arg0);
        case 26:
            return semWait((char *) arg0);
        default:
            return -1;
    }
}

int hardRead(uint64_t fd, char *buf, uint64_t count){
/*
    switch (fd) {
        case 1:{
            int idx = 0;
            while (idx < count && hasNext()) {
                processBuf(buf, &idx);
            }
            //alignPointers();
            return idx;
        }
    }
    */
    return 0;
}


void printPixel(uint64_t x, uint64_t y, uint32_t color) {
    putPixel(color, x + X_START_OFFSET, y + Y_START_OFFSET);
}

void getScreenDimensions(coords *ret) {
    ret->x = X_LIMIT-X_START_OFFSET;
    ret->y = Y_LIMIT-Y_START_OFFSET+8;
}

void ps(){
    drawString("\n", 2, 2 );
    ProcessInfoArray * p_list = getProcessArray();
    drawString("Active processes: ", 19, 2);
    char ps_len[5];
    uintToBase(p_list->length, ps_len, 10);
    drawString(ps_len, 5, 2);
    drawString("\n", 2, 2 );

    char p_pid[5], p_parent_pid[5], p_prio[2], p_rsb[17], p_rsp[17];
    for (int i = 0; i < p_list->length ; i++){
        uintToBase(p_list->array[i].pid, p_pid, 10);
        uintToBase(p_list->array[i].parent_pid, p_parent_pid, 10);
        uintToBase(p_list->array[i].piority, p_prio, 10);
        uintToBase((uint64_t) p_list->array[i].rsb, p_rsb, 16);
        uintToBase((uint64_t) p_list->array[i].rsp, p_rsp, 16);
        drawString("Process: ", 10, 2); drawStringWithColor(p_list->array[i].name, strLen(p_list->array[i].name), 0xffff00, 0x000000, 2) ; drawString("\n", 2, 2 );
        drawString("    PID: ", 10, 2); drawString(p_pid, 4, 2);
        drawString("        Parent PID: ", 22, 2) ; drawString((p_list->array[i].pid == 0) ? "-" : p_parent_pid, 5, 2) ;drawString("\n", 2, 2 );
        drawString("    Priority: ", 14, 2); drawString((p_list->array[i].pid == 0) ? "-" : p_prio, 2, 2); drawString("\n", 2, 2 );
        drawString("    RSB: #",11, 2); drawString(p_rsb, 16, 2);
        drawString("        RSP: #",16, 2); drawString(p_rsp, 16, 2); drawString("\n", 2, 2 );
        drawString(p_list->array[i].fg ? "    Foreground"  : "    Background", 15, 2);
        drawString("        State: ", 17, 2);
        switch (p_list->array[i].p_state) {
            case READY:
                drawStringWithColor("READY", 6,0x00ff00,0x000000 , 2);
                break;
            case RUNNING:
                drawStringWithColor("RUNNING", 8,0x0000ff,0x000000 , 2);
                break;
            case BLOCKED:
                drawStringWithColor("BLOCKED", 8,0xff0000,0x000000 , 2);
                break;
            default:
                drawString("TERMINATED", 10, 2);
        }
        drawString("\n", 2, 2 );
        memFree(p_list->array[i].name);
    }
    memFree(p_list->array);
    memFree(p_list);
}