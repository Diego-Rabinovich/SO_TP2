#include "../Shell/include/shellMain.h"
#include "include/userLibAsm.h"
#include "include/userLib.h"

int doNothing(int argc, char*argv[]){
    while(1);
}

int main(int argc, char*argv[]) {
    char * args[] = {0x00000000};
    sys_fork(shell, args , "shell", 3);
    sys_fork(doNothing, args , "doN", 3);

}