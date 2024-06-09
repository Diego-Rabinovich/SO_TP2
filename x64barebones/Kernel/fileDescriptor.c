//

#include "include/fileDescriptor.h"

#include "videoDriver.h"
#include "include/keyboardDriver.h"
#include "include/memoryManager.h"
#include "include/lib.h"
#include "include/scheduler.h"
#include "include/interrupts.h"
#define BUFF_SIZE 128
#define MAX_FDS 128
#define WAITING_NOTHING 0
#define WAITING_WRITE 1
#define WAITING_READ 2

typedef struct FileDescriptorCDT
{
    int16_t fd;
    unsigned char buff [BUFF_SIZE];
    unsigned char readIdx;
    unsigned char writeIdx;
    unsigned char used;
    // char locked_write;
    // char locked_read;
    //sem
} FileDescriptorCDT;


typedef struct ProcessWaiting{
    int16_t fd;
    unsigned char r_w;
}ProcessWaiting;

void processBuf(unsigned char * buf,int *idx,FileDescriptor fd);

ProcessWaiting processes_wating[MAX_PROCESSES]={0};
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

FileDescriptor initFd(){
    int16_t new_fd_idx= getNextFd();
    if(new_fd_idx==-1){
        return NULL;
    }
    FileDescriptor new_fd=memAlloc(sizeof (FileDescriptorCDT));
    fds[new_fd_idx]=new_fd;
    new_fd->fd=new_fd_idx;
    memset(new_fd->buff,0,BUFF_SIZE);
    new_fd->readIdx=0;
    new_fd->writeIdx=0;
    new_fd->used=0;
    // new_fd->locked_read=1;
    // new_fd->locked_write=0;
    return new_fd;
}

int getSubscribedAmount(int16_t fd, char r_w) {
    int ret = 0;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if(processes_wating[i].fd == fd && processes_wating[i].r_w == r_w) {
            ret++;
        }
    }
    return ret;
}

int writeOnFile(FileDescriptor fd,  char * buf, unsigned long len, uint32_t hexFontColor, uint32_t hexBGColor, uint32_t fontSize){
    if(fd==NULL){
        return -1;
    }
    char awoken = 0;
    switch (fd->fd){
        case STDOUT:
            drawStringWithColor(buf, len, hexFontColor, hexBGColor, fontSize);
            break;
        case STDERR:
            drawStringWithColor(buf, len, 0xff0000, 0x000000, fontSize);
            break;
        case DEV_NULL: break;
        default:
            for(int i=0;i<len;i++) {
                // if(fd->fd == STDIN)
                // {
                //     char wridx [3];
                //     uintToBase(fd->writeIdx, wridx, 10);
                //     drawStringWithColor(wridx, 3, 0xff0000, 0, 2);
                //
                //     char rdidx [3];
                //     uintToBase(fd->readIdx, rdidx, 10);
                //     drawStringWithColor(rdidx, 3, 0xffff00, 0, 2);
                // }
                if(fd->used >= BUFF_SIZE) {//pregunto si no tiene espacio
                    if(fd->fd == STDIN && getSubscribedAmount(STDIN, WAITING_READ) == 0) {
                        //Si se llena el buffer de STDIN y no hay lectores, vacialo
                        fd->used=0;
                        fd->writeIdx=0;
                        fd->readIdx=0;
                    } else
                    {
                        uint16_t pid = getPid();
                        processes_wating[pid].fd = fd->fd;
                        processes_wating[pid].r_w = WAITING_WRITE;
                        //drawStringWithColor("WAITING TO WRITE", 17, 0x00ff00, 0, 2);
                        setState(pid, BLOCKED);
                        // fd->locked_write = 1;
                        //     _sti();
                        //     while (fd->locked_write);
                        _cli();
                        awoken=0;
                        processes_wating[pid].fd = 0;
                        processes_wating[pid].r_w = WAITING_NOTHING;
                    }
                }
                fd->buff[fd->writeIdx]=buf[i];
                fd->writeIdx=(fd->writeIdx + 1)%BUFF_SIZE;
                fd->used++;
                if(!awoken){
                    awoken=1;
                    for(int j=0;j<MAX_PROCESSES;j++){
                        if(processes_wating[j].r_w==WAITING_READ&&processes_wating[j].fd==fd->fd){
                            setState(j,READY);
                            // fd->locked_read = 0;
                            break;
                        }
                    }
                }
            }
            break;
    }
    return (int)len;
}

int readOnFile(FileDescriptor fd, unsigned char * target, unsigned long len){
    if(fd==NULL || fd->fd == DEV_NULL){
        return -1;
    }

    char awokenWriters=0;
    for(int i=0;i<len;) {
        if(fd->used == 0) {
            uint16_t pid = getPid();
            processes_wating[pid].fd = fd->fd;
            processes_wating[pid].r_w = WAITING_READ;
            //drawStringWithColor(" Waiting to read ", 16, 0x00ff00, 0, 2);
            setState(pid, BLOCKED);
            //drawStringWithColor(" Done reading ", 16, 0xff0000, 0xffffff, 2);
            // fd->locked_read = 1;
            // _sti();
            // while (fd->locked_read);
            _cli();
            awokenWriters=0;
            processes_wating[pid].r_w = WAITING_NOTHING;
            processes_wating[pid].fd = 0;
        }
        if(fd->used > 0) {
            if(fd->fd==STDIN) {
                processBuf(target,&i,fd);
            } else {
                target[i]=fd->buff[fd->readIdx];
                fd->readIdx=(fd->readIdx+1)%BUFF_SIZE;
                i++;
                fd->used--;
            }
            if(!awokenWriters){
                awokenWriters=1;
                for(int j=0;j<MAX_PROCESSES;j++){
                    if(processes_wating[j].r_w==WAITING_WRITE&&processes_wating[j].fd==fd->fd){
                        setState(j,READY);
                        // fd->locked_write = 0;
                        break;
                    }
                }
            }
        }
    }
    return (int)len;
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
    fds[fd->fd]=NULL;
    count_fd--;
    memFree(fd);
}