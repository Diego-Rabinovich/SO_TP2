// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../Shell/include/shellMain.h"
#include "include/userLibAsm.h"
#include "include/userLib.h"


int main(int argc, char*argv[]) {
    char * args[] = {0x0000000};
    int16_t fds[3] = {STDIN, STDOUT, STDERR};
    sys_createProcess(shell, args , "sh", 3, fds);
    while (1) {
        sys_wait_pid(-1);
    }
}