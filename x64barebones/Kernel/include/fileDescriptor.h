#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include "stdint.h"

typedef struct FileDescriptorCDT *FileDescriptor;

int writeOnFile(FileDescriptor fd,char * buf, unsigned long len, uint32_t hexFontColor, uint32_t hexBGColor, uint32_t fontSize);
int readOnFile(FileDescriptor fd, unsigned char * target, unsigned long len);
FileDescriptor getFdByIdx(int16_t fd);
FileDescriptor getFdByName(char* name);
int16_t openFd(char* name);
int16_t createFd(char* name);
void closeFdByIdx(int16_t fd);
void closeFdByName(char* name);
void clearSTDIN();

#endif //FILEDESCRIPTOR_H
