// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// Created by ghigl on 11/06/2024.
//

#include "include/userApps.h"
#include "../Math/include/math.h"
#include "../User/include/userLib.h"
#include "../User/include/userLibAsm.h"

#define EOF 1

int help(int argc, char** argsv) {
    if (argc == 0) {
        print("\nHelp Menu:\n", 0xffffff, 2);
        print("To send a process 'p' to background use: p &\n", 0xff00ff, 2);
        print("To pipe process 'p1' into 'p2' use: p1 | p2\n", 0xff00ff, 2);
        print("Separate arguments with spaces\n", 0xffffff, 2);
        print("ID - SYNTAX - DESC\n", 0xffffff, 2);
        print("1 - help (optional) arg1 (options: math, test)\n", 0xffffff, 2);
        print("2 - math op left right - applies the op to left and right \n",0xffffff, 2);
        print("3 - clear\n", 0xffffff, 2);
        print("4 - fun - throws an invalid opCode exception\n", 0xffffff, 2);
        print("5 - time", 0xffffff, 2);
        print("6 - test arg0 (options: mm, sync, priority, processes) \n", 0xffffff, 2);
        print("7 - mem - shows mem state\n", 0xffffff, 2);
        print("8 - block pid arg1 (arg1 options: 0 (unblock), 1 (block))\n", 0xffffff, 2);
        print("8 - kill pid\n", 0xffffff, 2);
        print("8 - nice pid prio (prio options: 0-3)\n", 0xffffff, 2);
        print("9 - ps - list processes\n", 0xffffff, 2);
        print("10 - phylo - run phylo game\n", 0xffffff, 2);
        print("11 - ps - list processes\n", 0xffffff, 2);
        print("12 - cat\n", 0xffffff, 2);
        print("13 - filter - filter vocals\n", 0xffffff, 2);
        print("14 - wc - line count\n", 0xffffff, 2);
        print("15 - loop - prints pid and msg\n", 0xffffff, 2);
    } else if (argc == 1) {
        if (strCmp(argsv[0], "math") == 0) {
            print("\n Help Menu: Math\n\n", 0xffffff, 2);
            print("Use:", 0xffffff, 2);
            print(" ", 0xffffff, 2);
            print("ans", 0x12345678, 2);
            print(" as left or/and right to operate with the last calculated result\n\n", 0xffffff, 2);
            print("List of valid operations: \n", 0xffffff, 2);
            print("mul", 0x000000ff, 2);
            print(" - ex: math mul 1 3, returns 1*3=3\n", 0xffffff, 2);
            print("sub", 0x000000ff, 2);
            print(" - ex: math sub ans 3, returns (if ans = 3): 3-3=0\n", 0xffffff, 2);
            print("sum", 0x000000ff, 2);
            print(" - ex: math sum 1 3, returns 1+3=4\n", 0xffffff, 2);
            print("div", 0x000000ff, 2);
            print(" - ex: math div 3 3, returns 3/3=1. Note: div x 0 will throw exception\n", 0xffffff, 2);
            print("pot", 0x000000ff, 2);
            print(" - ex: math pot 1 3, returns 2^3=8\n", 0xffffff, 2);
            print("mod", 0x000000ff, 2);
            print(" - ex: math mod 10 5, returns 10%5=0\n", 0xffffff, 2);
        } else if (strCmp(argsv[0], "test") == 0){
            print("\nTest help Menu:\n", 0xffffff, 2);
            print("1 - test mm:\n", 0x00ff00, 2);
            print("Argument 1: request size (in MB)\n", 0xffffff, 2);
            print("Example: test mm 10\n", 0xffffff, 2);
            print("2 - test processes:\n", 0x00ff00, 2);
            print("Argument 1: process qty\n", 0xffffff, 2);
            print("Example: test processes 5\n", 0xffffff, 2);
            print("3 - test sync:\n", 0x00ff00, 2);
            print("Argument 1: increments\n", 0xffffff, 2);
            print("Argument 2: useSem? (0 or 1)\n", 0xffffff, 2);
            print("Example: test sync 5000 1\n", 0xffffff, 2);
            print("4 - test priority:\n", 0x00ff00, 2);
            print("Example: test priority\n", 0xffffff, 2);
        }
    }
    return 0;
}

int mathWrapper(int argc, char** argv) {
    signed int res = 0;
    if (math(argv[0], argv[1], argv[2], &res)) {
        char str[32];
        signedIntToBase(res, str, 10);

        char buff[256] = {0};
        int len = 0;
        len = appendStr(buff, "Result: ", len);
        appendStr(buff, str, len);
        print(buff, 0xffffff, 2);
    }
    return 0;
}

int time(int argc, char** argv) {
    int time[3];
    sys_getCpuTime(time);
    char buff[256] = {0};
    uint16_t len = 0;

    char str[3];
    uintToBase(time[0], str, 10);
    len = appendStr(buff, str, len);
    len = appendStr(buff, ":", len);

    uintToBase(time[1], str, 10);
    len = appendStr(buff, str, len);
    len = appendStr(buff, ":", len);

    uintToBase(time[2], str, 10);
    appendStr(buff, str, len);

    print(buff, 0xffffff, 2);
    return 0;
}

int filter(int argc, char** argv) {
    int i = 0, j = 0;
    char c;
    char chain[1024] = {0};
    int16_t fds[3];
    char toReturn[1024] = {0};
    sys_get_FDs(fds);
    sys_read(fds[STDIN], (char*)chain, 1);
    while (i < 1024&&(c = chain[i]) != EOF) {
        if (!(c & 0x80) && c != 'a' && c != 'A' && c != 'e' && c != 'E' && c != 'i' && c != 'I' && c != 'o' && c != 'O'
            && c != 'u' && c != 'U') {
            toReturn[j++] = chain[i];
        }
        i++;
        sys_read(fds[STDIN], chain + i, 1);
    }

    toReturn[j] = '\0';
    printUpToEOF(toReturn, 0xffffff, 2);
    return 0;
}

int cat(int argc, char** argv) {
    int i = 0, j = 0;
    char c;
    char chain[1024] = {0};
    int16_t fds[3];
    char toReturn[1024] = {0};
    sys_get_FDs(fds);
    sys_read(fds[STDIN], (char*)chain, 1);
    while (i < 1024 && (c = chain[i]) != EOF ) {
        if (!(c & 0x80)) {
            toReturn[j++] = chain[i];
        }
        i++;
        sys_read(fds[STDIN], chain + i, 1);
    }

    toReturn[j] = '\0';
    printUpToEOF(toReturn, 0xffffff, 2);
    return 0;
}

int wc(int argc, char** argv) {
    int i = 0;
    char c;
    char chain[2] = {0};

    int16_t fds[3];
    sys_get_FDs(fds);

    sys_read(fds[STDIN], chain, 1);
    while ((c = chain[0]) != EOF) {
        if (c == '\n') {
            i++;
        }
        sys_read(fds[STDIN], chain, 1);
    }

    char ret[25] = {0};
    uintToBase(i, ret, 10);
    print(ret, 0xffffff, 2);
    return 0;
}

int killWrapper(int argc, char** argv) {
    int pid;
    strToInt(argv[0], &pid);
    if (sys_kill(pid) == 0) {
        char msg[20] = {0};
        concat("Killed: ", argv[0], msg);
        print(msg, 0xffffff, 2);
    }
    else {
        printErr("Failed to kill: ", 2);
        return -1;
    }
    return 0;
}

int blockWrapper(int argc, char** argv) {
    int pid;
    strToInt(argv[0], &pid);
    int ret = -1;
    int is0 = strCmp(argv[1], "0") == 0;


    if (is0 == 0) {
        ret = sys_unblock(pid);
    }
    else {
        ret = sys_block(pid);
    }
    if (ret != -1) {
        char msg[20] = {0};
        concat(is0 ? "Unblocked: " : "\nBlocked: ", argv[0], msg);
        print(msg, 0xffffff, 2);
    }
    else {
        printErr("Block fail", 2);
        return -1;
    }
    return 0;
}

int niceWrapper(int argc, char** argv) {
    int pid;
    int priority;
    strToInt(argv[0], &pid);
    strToInt(argv[1], &priority);

    if (sys_nice(pid, priority) == priority) {
        char msg[50] = {0};
        uint32_t len = 0;

        len = appendStr(msg, "Changed process: ", len);
        len = appendStr(msg, argv[0], len);
        len = appendStr(msg, " priority to ", len);
        appendStr(msg, argv[1], len);
        print(msg, 0xffffff, 2);
    }
    else {
        printErr("Nice fail", 2);
        return -1;
    }
    return 0;
}

int loop(int argc, char** argv) {
    int millis;
    strToInt(argv[0], &millis);

    char pid_str[4];
    uintToBase(sys_pid(), pid_str, 10);
    char msg [50];
    concat("\nHello, I'm process number ", pid_str, msg);
    while (1) {
        print(msg, 0xffffff, 2);
        sys_waitNMillis(millis);
    }
}
