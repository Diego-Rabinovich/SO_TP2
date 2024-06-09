#include <stdint.h>
#include "../../User/include/userLib.h"
#ifndef TPEARQUI_SHELLLIB_H
#define TPEARQUI_SHELLLIB_H

#define NOT_USABLE 0
#define ESC 1
#define CTRL 2
#define SHIFT 3
#define CAPS 4
#define ALT 5
#define LEFT_ARROW 10
#define RIGHT_ARROW 11
#define UP_ARROW 12
#define DOWN_ARROW 13
#define BUFF_SIZE 128 //tiene que ser par
#define MAX_ARGS ( BUFF_SIZE / 2 )
void newLine(uint32_t fontSize);
char isSpecial(char c);
int parseArgs(const unsigned char *buffer,char args[MAX_ARGS][BUFF_SIZE], int size, char parser, char end);
void commandNotFound(uint32_t fontSize);
#endif //TPEARQUI_SHELLLIB_H
