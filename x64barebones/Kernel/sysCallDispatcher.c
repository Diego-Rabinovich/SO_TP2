// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/sysCallDispatcher.h"
#include <stdint.h>
#include "include/videoDriver.h"
#include "include/lib.h"
#include "include/audioDriver.h"
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
void printMemInfo();

uint64_t sysCallDispatcher(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
                      uint64_t arg4, uint64_t arg5, uint64_t RAX) {
    switch (RAX) {
        case 0:
            return readOnFile((int16_t)arg0, (unsigned char *) arg1, arg2);
        case 1:
            return writeOnFile((int16_t)arg0, (unsigned char *) arg1, arg2, arg3, arg4, arg5);
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
            return RAX;
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
            yieldNoSti();
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
        case 27:
            return openFd((char*)arg0, arg1);
        case 28:
            closeFd(arg0);
            return RAX;
        case 29:
            return openFdForPID((char *)arg0, arg1, arg2);
        case 30:
            printMemInfo();
            return RAX;
        case 31:
            closeFdForPID(arg0, arg1);
            return RAX;
        case 32:
            changeFdWriter(arg0, arg1);
            return RAX;
        case 33:
            changeFdReader(arg0, arg1);
            return RAX;
        default:
            return -1;
    }
}


void printPixel(uint64_t x, uint64_t y, uint32_t color) {
    putPixel(color, x + X_START_OFFSET, y + Y_START_OFFSET);
}

void getScreenDimensions(coords *ret) {
    ret->x = X_LIMIT-X_START_OFFSET;
    ret->y = Y_LIMIT-Y_START_OFFSET+8;
}

void ps(){
    int16_t fds[3];
    getFDs(fds);
    int16_t fd = fds[STDOUT];
    writeOnFile(fd,(unsigned char *) "\n", 2, 0xffffff,0,2 );
    ProcessInfoArray * p_list = getProcessArray();
    writeOnFile(fd, (unsigned char *) "Active processes: ", 19,0xffffff,0, 2);
    char ps_len[5];
    uintToBase(p_list->length, ps_len, 10);
    writeOnFile(fd,(unsigned char *) ps_len, 5,0xffffff,0, 2);
    writeOnFile(fd, (unsigned char *)"\n", 2,0xffffff,0, 2 );
    char p_pid[5], p_parent_pid[5], p_prio[2], p_rsb[17], p_rsp[17];
    for (int i = 0; i < p_list->length ; i++){
        uintToBase(p_list->array[i].pid, p_pid, 10);
        uintToBase(p_list->array[i].parent_pid, p_parent_pid, 10);
        uintToBase(p_list->array[i].piority, p_prio, 10);
        uintToBase((uint64_t) p_list->array[i].rsb, p_rsb, 16);
        uintToBase((uint64_t) p_list->array[i].rsp, p_rsp, 16);
        writeOnFile(fd, (unsigned char *)"Process: ", 10, 0xffffff, 0, 2);
        writeOnFile(fd,(unsigned  char *) p_list->array[i].name, strLen(p_list->array[i].name)+1, 0xffff00, 0x000000, 2);
        writeOnFile(fd,(unsigned  char *)"\n", 1, 0xffffff, 0, 2);
        writeOnFile(fd,(unsigned  char *) "    PID: ", 10, 0xffffff, 0, 2);
        writeOnFile(fd,(unsigned  char *) p_pid, strLen(p_pid) +1, 0xffffff, 0, 2);
        writeOnFile(fd,(unsigned  char *) "        Parent PID: ", 21, 0xffffff , 0, 2) ;
        writeOnFile(fd, (p_list->array[i].pid == 0) ? (unsigned  char *) "-" :(unsigned  char *) p_parent_pid, (p_list->array[i].pid == 0) ? 2 : strLen(p_parent_pid),0xffffff,0, 2) ;
        writeOnFile(fd,(unsigned  char *) "\n", 2, 0xffffff, 0, 2 );
        writeOnFile(fd,(unsigned  char *)"    Priority: ",14, 0xffffff ,0, 2);
        writeOnFile(fd, (p_list->array[i].pid == 0) ? (unsigned  char *)"-" : (unsigned  char *)p_prio, 2, 0xffffff,0,2);
        writeOnFile(fd, (unsigned  char *)"\n", 2, 0xffffff, 0, 2);
        writeOnFile(fd,(unsigned  char *)"    RSB: #",11,0xffffff,0, 2);
        writeOnFile(fd,(unsigned  char *) p_rsb, strLen(p_rsb)+1, 0xffffff, 0, 2);
        writeOnFile(fd,(unsigned  char *) "        RSP: #",15,0xffffff,0, 2);
        writeOnFile(fd, (unsigned  char *) p_rsp, strLen(p_rsp)+1,0xffffff,0, 2);
        writeOnFile(fd, (unsigned  char *)"\n", 2,0xffffff,0, 2 );
        writeOnFile(fd, p_list->array[i].fg ? (unsigned  char *)"    Foreground"  : (unsigned  char *)"    Background", 15,0xffffff,0, 2);
        writeOnFile(fd,(unsigned  char *) "        State: ", 16,0xffffff,0, 2);
        switch (p_list->array[i].p_state) {
            case READY:
                writeOnFile(fd,(unsigned  char *) "READY", 6,0x00ff00,0x000000 , 2);
                break;
            case RUNNING:
                writeOnFile(fd,(unsigned  char *) "RUNNING", 8,0x0000ff,0x000000 , 2);
                break;
            case BLOCKED:
                writeOnFile(fd,(unsigned  char *) "BLOCKED", 8,0xff0000,0x000000 , 2);
                break;
            default:
                writeOnFile(fd,(unsigned  char *) "TERMINATED", 10,0xffffff, 0, 2);
        }
        writeOnFile(fd, (unsigned  char *) "\n", 2,0xfffff,0, 2 );
        memFree(p_list->array[i].name);
    }
    memFree(p_list->array);
    memFree(p_list);
}

void printMemInfo(){
    MemoryInfo * info = getMemInfo();
    int16_t fds[3];
    getFDs(fds);
    int16_t fd = fds[STDOUT];
    char total_str[20], reserved_str[20], free_str[20];
    uintToBase(info->total, total_str, 10);
    uintToBase(info->reserved, reserved_str, 10);
    uintToBase(info->free, free_str, 10);
    writeOnFile(fd, (unsigned char *)"\nMemory Info: \n", 17, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)"TOTAL (b): ", 11, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)total_str, strLen(total_str)+1, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)"\n", 1, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)"RESERVED (b): ", 14, 0xffff00, 0, 2);
    writeOnFile(fd, (unsigned char *)reserved_str, strLen(reserved_str)+1, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)"\n", 1, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)"FREE (b): ", 10, 0x00ff00, 0, 2);
    writeOnFile(fd, (unsigned char *)free_str, strLen(free_str)+1, 0xffffff, 0, 2);
    writeOnFile(fd, (unsigned char *)"\n", 1, 0xffffff, 0, 2);
}