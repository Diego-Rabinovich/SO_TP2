#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include "stdint.h"

typedef struct FileDescriptorCDT *FileDescriptor;

int writeOnFile(FileDescriptor fd, const char * buf, unsigned long len);
int readOnFile(FileDescriptor fd, char * target, unsigned long len);
int16_t getFlioNo(FileDescriptor fd);

#endif //FILEDESCRIPTOR_H
