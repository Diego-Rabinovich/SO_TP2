#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include "stdint.h"


int writeOnFile(int16_t fd, unsigned char * buf, unsigned long len, uint32_t hexFontColor, uint32_t hexBGColor, uint32_t fontSize);
int readOnFile(int16_t fd, unsigned char * target, unsigned long len);
int16_t openFd(char *name, char mode);
int16_t openFdForPID(char *name, int16_t pid, char mode);
void closeFd(int16_t fd);
void closeFdForPID(int16_t fd, int16_t pid);
void changeFdWriter(int16_t fd, int16_t new_pid);
void changeFdReader(int16_t fd, int16_t new_pid);

void clearSTDIN();
void initiateFDs();

#endif //FILEDESCRIPTOR_H
