// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/fileDescriptor.h"

#include "videoDriver.h"
#include "include/keyboardDriver.h"
#include "include/memoryManager.h"
#include "include/lib.h"
#include "include/scheduler.h"
#include "include/semaphores.h"

#define EOF 1
#define BUFF_SIZE 128
#define MAX_FDS 2048

#define WRITER 'w'
#define READER 'r'


typedef struct FileDescriptorCDT
{
    int16_t fd_idx;
    char name[MAX_NAME];
    unsigned char buff[BUFF_SIZE];
    unsigned char readIdx;
    unsigned char writeIdx;
    unsigned char used;
    char semReadName[MAX_NAME];
    char semWriteName[MAX_NAME];
    char mutexName[MAX_NAME];
    int16_t writer_pid;
    int16_t reader_pid;
} FileDescriptorCDT;

typedef FileDescriptorCDT *FileDescriptor;

void processBuf(unsigned char *buf, int *idx, FileDescriptor fd);
FileDescriptor initFd();
void freeFd(FileDescriptor fd);
FileDescriptor getFdByIdx(int16_t fd);
FileDescriptor getFdByName(char* name);
int16_t createFd(char *name);

FileDescriptor fds[MAX_FDS] = {0};
int16_t next_fd = 0;
int16_t count_fd = 0;

char *printMutex = "print_mutex";

void initiateFDs() {
    newSemaphore(printMutex, 1);
    openFdForPID("stdin", 0, 'w');
    openFdForPID("stdout", 0, 'r');
    openFdForPID("stderr", 0, 'r');
}

int16_t getNextFdIdx() {
    if (count_fd >= MAX_FDS) {
        return -1;
    }
    while (fds[next_fd] != NULL) {
        next_fd = (int16_t)((next_fd + 1) % MAX_FDS);
    }
    return next_fd;
}

FileDescriptor getFdByName(char *name) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (fds[i] != NULL && strCmp(fds[i]->name, name) == 0) {
            return fds[i];
        }
    }
    return NULL;
}

void clearSTDIN() {
    FileDescriptor stdin = fds[0];
    stdin->readIdx = stdin->writeIdx;
    for (int i = 0; i < stdin->used; ++i) { //Empty left-overs
        semWait(stdin->semReadName);
    }
    stdin->used = 0;
}

int16_t createFd(char *name) {
    uint8_t name_len = strLen(name);

    if (count_fd >= MAX_FDS || name_len >= MAX_NAME || name_len <= 0) return -1;

    FileDescriptor fd = getFdByName(name);
    if (fd == NULL) {
        fd = initFd();
        if (fd == NULL) return -1;
        memcpy(fd->name, name, name_len + 1);
        return fd->fd_idx;
    }
    return -1;
}

int16_t openFdForPID(char *name, int16_t pid, char mode) {
    FileDescriptor fd = getFdByName(name);
    if (fd == NULL) {
        int16_t idx;
        if((idx = createFd(name)) == -1) {
            return -1;
        }
        fd = getFdByIdx(idx);
    }

    if((fd->writer_pid != -1 && mode==WRITER) || (fd->reader_pid != -1 && mode==READER)) {  //PIPE ALREADY OCCUPIED
        return -1;
    }

    if(mode == WRITER) {
        fd->writer_pid = pid;
    } else if(mode == READER) {
        fd->reader_pid = pid;
    } else {
        return -1;   //INVALID MODE
    }

    return fd->fd_idx;
}

void changeFdReader(int16_t fd, int16_t new_pid) {
    FileDescriptor fd_obj = getFdByIdx(fd);
    if(fd_obj == NULL) return;
    fd_obj->reader_pid = new_pid;
}

void changeFdWriter(int16_t fd, int16_t new_pid) {
    FileDescriptor fd_obj = getFdByIdx(fd);
    if(fd_obj == NULL) return;
    fd_obj->writer_pid = new_pid;
}

int16_t openFd(char* name, char mode) {
    return openFdForPID(name, getPid(), mode);
}

void closeFd(int16_t fd) {
    closeFdForPID(fd, getPid());
}

void closeFdForPID(int16_t fd, int16_t pid) {
    FileDescriptor fd_obj = getFdByIdx(fd);
    if(fd_obj == NULL) return;
    if(fd_obj->writer_pid  == pid) {
        writeOnFile(fd_obj->fd_idx, (unsigned char *)"\1", 1, 0,0,0);
        fd_obj->writer_pid = -1;
    }
    if(fd_obj->reader_pid == pid) {
        fd_obj->reader_pid = -1;
    }

    if(fd_obj->reader_pid == -1 && fd_obj->writer_pid == -1) {
        freeFd(fd_obj);
    }
}

FileDescriptor initFd() {
    int16_t new_fd_idx = getNextFdIdx();
    if (new_fd_idx == -1) {
        return NULL;
    }
    FileDescriptor new_fd = memAlloc(sizeof(FileDescriptorCDT));
    if (new_fd == NULL) return NULL;
    fds[new_fd_idx] = new_fd;
    new_fd->fd_idx = new_fd_idx;
    memset(new_fd->buff, 0, BUFF_SIZE);
    new_fd->readIdx = 0;
    new_fd->writeIdx = 0;
    new_fd->used = 0;
    new_fd->writer_pid = -1;
    new_fd->reader_pid = -1;
    char idxStr[10];
    uintToBase(new_fd_idx, idxStr, 10);
    memcpy(new_fd->mutexName, "mutex_fd_", 9);
    memcpy((void *) new_fd->mutexName + 9, idxStr, strLen(idxStr) + 1);
    memcpy(new_fd->semReadName, "read_fd_", 8);
    memcpy((void *) new_fd->semReadName + 8, idxStr, strLen(idxStr) + 1);
    memcpy(new_fd->semWriteName, "write_fd_", 9);
    memcpy((void *) new_fd->semWriteName + 9, idxStr, strLen(idxStr) + 1);
    newSemaphore(new_fd->mutexName, 1);
    newSemaphore(new_fd->semReadName, 0);
    newSemaphore(new_fd->semWriteName, BUFF_SIZE);
    return new_fd;
}

int writeOnFile(int16_t fd_idx, unsigned char *buff, unsigned long len, uint32_t hexFontColor, uint32_t hexBGColor, uint32_t fontSize) {
    FileDescriptor fd = getFdByIdx(fd_idx);
    if (fd == NULL || len == 0) {
        return -1;
    }
    switch (fd->fd_idx) {
        case STDOUT:
            semWait(printMutex);
            drawStringWithColor((char *)buff, len, hexFontColor, hexBGColor, fontSize);
            semPost(printMutex);
            break;
        case STDERR:
            semWait(printMutex);
            drawStringWithColor((char *)buff, len, 0xff0000, 0x000000, fontSize);
            semPost(printMutex);
            break;
        case DEV_NULL:
            break;
        case STDIN:
            for (int i = 0; i < len; i++) {
                semWait(fd->mutexName);
                fd->buff[fd->writeIdx] = buff[i];
                fd->writeIdx = (fd->writeIdx + 1) % BUFF_SIZE;
                if (fd->used < BUFF_SIZE) {
                    fd->used++;
                    semPost(fd->semReadName);
                } else {
                    fd->readIdx = fd->writeIdx;
                }
                semWait(printMutex);
                semPost(printMutex);
                semPost(fd->mutexName);
            }
            break;
        default:
            if(fd->writer_pid != getPid()) return -1;

            for (int i = 0; i < len; i++) {
                semWait(fd->semWriteName);
                semWait(fd->mutexName);
                fd->buff[fd->writeIdx] = buff[i];
                fd->writeIdx = (fd->writeIdx + 1) % BUFF_SIZE;
                fd->used++;
                semPost(fd->semReadName);
                semPost(fd->mutexName);
            }
            break;
    }
    return (int)len;
}

int readOnFile(int16_t fd_idx, unsigned char *target, unsigned long len) {
    FileDescriptor fd = getFdByIdx(fd_idx);

    if (fd == NULL || fd->reader_pid != getPid()) {
        return -1;
    }
    if (fd->fd_idx == STDIN) {
        for (int i = 0; i < len;) {
            semWait(fd->semReadName);
            semWait(fd->mutexName);
            processBuf(target, &i, fd);
            semPost(fd->mutexName);
        }
    } else {
        for (int i = 0; i < len;) {
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
    return (int)len;
}

FileDescriptor getFdByIdx(int16_t fd) {
    if (fd >= MAX_FDS || fd < 0) {
        return NULL;
    }
    return fds[fd];
}

void processBuf(unsigned char *buf, int *idx, FileDescriptor fd) {
    unsigned char code = fd->buff[fd->readIdx];
    fd->readIdx = (fd->readIdx + 1) % BUFF_SIZE;
    fd->used--;
    if (code == EOF) {
        buf[(*idx)++] = code;
        return;
    }
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

void freeFd(FileDescriptor fd) {
    semDestroy(fd->mutexName);
    semDestroy(fd->semReadName);
    semDestroy(fd->semWriteName);
    fds[fd->fd_idx] = NULL;
    count_fd--;
    memFree(fd);
}
