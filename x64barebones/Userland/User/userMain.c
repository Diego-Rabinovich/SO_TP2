#include "../Shell/include/shellMain.h"
#include "include/userLibAsm.h"
#include "include/userLib.h"


int main(int argc, char*argv[]) {
    char * args[] = {0x00000000};
    int16_t fds[3] = {STDIN, STDOUT, STDERR};
    sys_createProcess(shell, args , "sh", 3, fds);
    while (1) {
        sys_wait_pid(-1);
    }
}