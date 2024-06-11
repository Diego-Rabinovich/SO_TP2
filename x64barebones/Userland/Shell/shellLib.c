// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/shellLib.h"

void newLine(uint32_t fontSize) {
    print("\n$>", 0xffffff, fontSize);
}

char isSpecial(char c) {
    return NOT_USABLE <= c && c <= DOWN_ARROW;
}

int parseArgs(const unsigned char *buffer, char args[MAX_ARGS][BUFF_SIZE], int size, char parser, char end) {
    int i = 0, idJ = 0, idX = 0;
    if (size > 0) {
        do {
            if (buffer[i] != parser && buffer[i] != end) {
                args[idX][idJ++] = buffer[i];
            } else if (idJ) {
                args[idX++][idJ] = 0;
                idJ = 0;
            }
            i++;
        } while (buffer[i] != 0 && buffer[i] != end && idX < size);
        if (idJ) {
            args[idX++][idJ] = 0;
        }
    }
    return idX;
}

void commandNotFound(uint32_t fontSize){
    sys_write(2, "\nCommand not found\n",20,0xffffff, 0, fontSize);
}