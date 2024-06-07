#include "../Shell/include/shellMain.h"
#include "include/userLibAsm.h"
#include "include/userLib.h"


int main(int argc, char*argv[]) {
    char * args[] = {0x00000000};
    return sys_wait_pid(sys_fork(shell, args , "sh", 3));
}