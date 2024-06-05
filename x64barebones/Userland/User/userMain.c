#include "../Shell/include/shellMain.h"
#include "include/userLibAsm.h"
int main(int argc, char*argv[]) {
    sys_fork(&shell, argv, "shell", 3)
    while (1);
}