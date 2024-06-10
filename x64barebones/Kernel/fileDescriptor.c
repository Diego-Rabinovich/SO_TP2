//

#include "include/fileDescriptor.h"

#include "videoDriver.h"
#include "include/keyboardDriver.h"
#include "include/memoryManager.h"
#include "include/lib.h"
#include "include/scheduler.h"
#include "include/interrupts.h"
#include "include/semaphores.h"

#define BUFF_SIZE 128
#define MAX_FDS 128
#define WAITING_NOTHING 0
#define WAITING_WRITE 1
#define WAITING_READ 2

typedef struct FileDescriptorCDT
{
    int16_t fd_idx;
    unsigned char buff [BUFF_SIZE];
    unsigned char readIdx;
    unsigned char writeIdx;
    unsigned char used;
    char semReadName[MAX_NAME];
    char semWriteName[MAX_NAME];
    char mutexName[MAX_NAME];
} FileDescriptorCDT;


typedef struct ProcessWaiting{
    int16_t fd;
    unsigned char r_w;
}ProcessWaiting;

void processBuf(unsigned char * buf,int *idx,FileDescriptor fd);

FileDescriptor fds[MAX_FDS]={0};
int16_t next_fd=0;
int16_t count_fd=0;

int16_t getNextFd(){
    if(count_fd>=MAX_FDS){
        return -1;
    }
    while (fds[next_fd]!=NULL){
        next_fd=(int16_t)((next_fd+1)%MAX_FDS);
    }
    return next_fd;
}

void clearSTDIN() {
    FileDescriptor stdin = fds[0];
    stdin->readIdx = stdin->writeIdx;
    for (int i = 0; i < stdin->used; ++i) { //Empty left-overs
        semWait(stdin->semReadName);
    }
    stdin->used = 0;
}

int16_t openFd() {
    FileDescriptor fd = initFd();
    if(fd == NULL) return -1;
    return fd->fd_idx;
}

void closeFd(int16_t fd){
    FileDescriptor fd_obj;
    if(fd >= DEFAULT_FDS && (fd_obj = getFd(fd)) != NULL) {
        freeFd(fd_obj);
    }
}

FileDescriptor initFd(){
    int16_t new_fd_idx= getNextFd();
    if(new_fd_idx==-1){
        return NULL;
    }
    FileDescriptor new_fd=memAlloc(sizeof (FileDescriptorCDT));
    fds[new_fd_idx]=new_fd;
    new_fd->fd_idx=new_fd_idx;
    memset(new_fd->buff,0,BUFF_SIZE);
    new_fd->readIdx=0;
    new_fd->writeIdx=0;
    new_fd->used=0;
    char idxStr[10];
    uintToBase(new_fd_idx, idxStr, 10);
    memcpy(new_fd->mutexName, "mutex_fd_", 9);
    memcpy((void *) new_fd->mutexName + 9, idxStr, strLen(idxStr)+1);
    memcpy(new_fd->semReadName, "read_fd_", 8);
    memcpy((void *) new_fd->semReadName + 8, idxStr, strLen(idxStr)+1);
    memcpy(new_fd->mutexName, "write_fd_", 9);
    memcpy((void *) new_fd->mutexName + 9, idxStr, strLen(idxStr)+1);
    newSemaphore(new_fd->mutexName, 1);
    newSemaphore(new_fd->semReadName, 0);
    newSemaphore(new_fd->semWriteName, BUFF_SIZE);
    return new_fd;
}

int writeOnFile(FileDescriptor fd,  char * buff, unsigned long len, uint32_t hexFontColor, uint32_t hexBGColor, uint32_t fontSize){
    if(fd==NULL){
        return -1;
    }
    switch (fd->fd_idx){
        case STDOUT:
            drawStringWithColor(buff, len, hexFontColor, hexBGColor, fontSize);
            break;
        case STDERR:
            drawStringWithColor(buff, len, 0xff0000, 0x000000, fontSize);
            break;
        case DEV_NULL: break;
        case STDIN:
            for(int i=0;i<len;i++) {
                semWait(fd->mutexName);
                fd->buff[fd->writeIdx] = buff[i];
                fd->writeIdx = (fd->writeIdx + 1) % BUFF_SIZE;
                i++;
                if(fd->used < BUFF_SIZE) {
                    fd->used++;
                    semPost(fd->semReadName);
                } else {
                    fd->readIdx = fd->writeIdx;
                }
                semPost(fd->mutexName);
            }
            break;
        default:
            for(int i=0;i<len;i++) {
                semWait(fd->semWriteName);
                semWait(fd->mutexName);
                fd->buff[fd->writeIdx] = buff[i];
                fd->writeIdx = (fd->writeIdx + 1) % BUFF_SIZE;
                i++;
                fd->used++;
                semPost(fd->semReadName);
                semPost(fd->mutexName);
            }
            break;
    }
    return len;
}

int readOnFile(FileDescriptor fd, unsigned char * target, unsigned long len){
    if(fd==NULL || fd->fd_idx == DEV_NULL){
        return -1;
    }
    if(fd->fd_idx == STDIN) {
        for(int i=0;i<len;) {
            semWait(fd->semReadName);
            semWait(fd->mutexName);
                processBuf(target, &i, fd);
            semPost(fd->mutexName);
        }
    } else {
        for(int i=0;i<len;) {
            semWait(fd->semReadName);
            semWait(fd->mutexName);
            target[i] = fd->buff[fd->readIdx];
            fd->readIdx = (fd->readIdx + 1) % BUFF_SIZE;
            i++;
            fd->used--;
            semPost(fd->semWriteName);
            semPost(fd->mutexName);
        }
    }
    return len;
}

FileDescriptor getFd(int16_t fd){
    if(fd>=MAX_FDS||fd<0){
        return NULL;
    }
    return fds[fd];
}


void processBuf(unsigned char* buf,int *idx,FileDescriptor fd){
    unsigned char code=fd->buff[fd->readIdx];
    fd->readIdx=(fd->readIdx+1)%BUFF_SIZE;
    fd->used--;
    if (code == E_KEYS) {
        code =fd->buff[fd->readIdx];
        fd->readIdx=(fd->readIdx+1)%BUFF_SIZE;
        fd->used--;
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
        unsigned char c = getStringFromCode(code);

        if (c != NOT_USABLE) {
            if (flag) {
                c += RELEASE;
            }
            buf[(*idx)++] = c;
        }
    }
}

void freeFd(FileDescriptor fd){
    fds[fd->fd_idx]=NULL;
    count_fd--;
    memFree(fd);
}