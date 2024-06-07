#include "../Shell/include/shellMain.h"
#include "include/userLibAsm.h"
#include "include/userLib.h"


int main(int argc, char*argv[]) {
    char * args[] = {0x00000000};
    sys_fork(shell, args , "sh", 3);
    while (1) {
        sys_wait_pid(-1);
    }
}