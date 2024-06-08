#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include "stdint.h"

typedef struct FileDescriptorCDT *FileDescriptor;

int writeOnFile(FileDescriptor fd, const unsigned char * buf, unsigned long len, uint32_t hexFontColor, uint32_t hexBGColor, uint32_t fontSize);
int readOnFile(FileDescriptor fd, unsigned char * target, unsigned long len);
FileDescriptor initFd();
FileDescriptor getFd(int16_t fd);
void freeFd(FileDescriptor fd);

#endif //FILEDESCRIPTOR_H
