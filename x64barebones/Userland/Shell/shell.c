// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* shell.c */
#include "../User/include/userLib.h"
#include "include/shellLib.h"
#include "include/shellMain.h"
#include "include/commands.h"
#include "include/userApps.h"
#include "../Testing/testing.h"
#include "../Philosophers/include/philosophers.h"
#define CTRL_PRESS 2
#define CTRL_RELEASE 0x82
#define NULL 0x00000000
#define EOF 1

unsigned char buffer[BUFF_SIZE] = {0};
char arguments[MAX_ARGS][BUFF_SIZE];
int idx = 0;
int fontSize = 2;
int fontColor = 0xffffff;
const int shellFontColor = 0xffffff;
uint32_t fdCounter = 0;

Coords dims;

typedef struct Command {
    char* p_name;
    Main function;
    char* args[MAX_ARGS + 1]; //null terminated
    int16_t fds[3];
    char* errMsg;
} Command;

// region signatures
Command getCommand(int argsCount, char args[MAX_ARGS][BUFF_SIZE]);
int prepareCommands(Command commands[2]);

//end-region


void clear();

char startCommand();

void header() {
    print("Welcome to PEPE OS\n", shellFontColor, fontSize);
    newLine(fontSize);
}

void headerNoNewLine() {
    print("Welcome to PEPE OS\n", shellFontColor, fontSize);
}

void resetBuffer() {
    for (int i = 0; i < BUFF_SIZE; i++) {
        buffer[i] = 0;
    }
    idx = 0;
}

int16_t fg_fds[3] = {STDIN, STDOUT, STDERR};

int shell(int argc, char** args) {
    unsigned char ctrl_press = 0;
    sys_getScreenDimensions(&dims);
    clear();
    newLine(2);
    while (1) {
        int size = sys_read(0, (char*)buffer + idx, 1);
        if (size > 0) {
            if (buffer[idx] == CTRL_PRESS) {
                ctrl_press = 1;
            }
            else if (buffer[idx] == CTRL_RELEASE) {
                ctrl_press = 0;
            }
            else if (!(buffer[idx] & 0x80) && !ctrl_press) {
                switch (buffer[idx]) {
                case '\n':
                    {
                        startCommand();
                        newLine(fontSize);
                        resetBuffer();
                        break;
                    }
                case '\b':
                    buffer[idx] = 0;
                    if (idx > 0) {
                        idx--;
                        print("\b", 0xffffff, fontSize);
                    }
                    break;
                case '\t':
                    for (int i = 0; i < 4; i++) {
                        buffer[idx++] = ' ';
                        print((char*)buffer + idx - 1, 0, fontSize);
                    }
                    break;
                default:
                    if (idx + 1 < BUFF_SIZE && !isSpecial(buffer[idx])) {
                        print((char*)buffer + idx, fontColor, fontSize);
                        idx++;
                    }
                    else if (idx + 1 < BUFF_SIZE) {
                        buffer[idx] = 0; //borro las especiales que no nos importan aquí
                    }
                    break;
                }
            }
        }
    }
}


char startCommand() {
    Command commands[2];
    int cmd = prepareCommands(commands);

    for (int i = 0; i < cmd; i++) {
        if (strCmp(commands[i].p_name, "N") == 0) {
            return 0;
        }
        if (commands[i].function == NULL) {
            printErr(commands[i].errMsg, 2);
            return 0;
        }
    }
    print("\n", 0, 2);
    if (cmd == 1) {
        int16_t pid = sys_createProcess(commands[0].function, commands[0].args, commands[0].p_name, 3, commands[0].fds);
        if (commands[0].fds[STDIN] == STDIN) {
            sys_wait_pid(pid);
        }
    }
    else {
        char idxFdStr[10];
        char fdName[24];
        uintToBase(fdCounter, idxFdStr, 10);
        concat("sh_fd_", idxFdStr, fdName);
        fdCounter++;
        int16_t fd = sys_createFd(fdName);
        commands[0].fds[STDOUT] = fd;
        commands[1].fds[STDIN] = fd;
        sys_createProcess(commands[0].function, commands[0].args, commands[0].p_name, 3, commands[0].fds);
        sys_createProcess(commands[1].function, commands[1].args, commands[1].p_name, 3, commands[1].fds);
        if (commands[0].fds[STDIN] == STDIN) {
            for(int i = 0; i < 2; i++) {
                sys_wait_pid(-1);
            }
        }
    }
    return 0;
}

void clear() {
    sys_blankArea(0, dims.x, 0, dims.y, 0);
    sys_resetCursor();
    headerNoNewLine();
}

int hasPipe(int argc) {
    for (int i = 0; i < argc; i++) {
        if (strCmp(arguments[i], "|") == 0) {
            return i;
        }
    }
    return -1;
}

int prepareCommands(Command commands[2]) {
    int argsCount = parseArgs(buffer, arguments, MAX_ARGS, ' ', '\n');
    int pipe = hasPipe(argsCount);
    if (pipe > 0) {
        char argAux[MAX_ARGS][BUFF_SIZE];
        for (int i = 0, j=pipe + 1; i < MAX_ARGS && arguments[j][0] != 0 ; i++, j++) {
            int k = 0;
            for (k = 0; arguments[j][k]; k++) {
                argAux[i][k] = arguments[j][k];
            }
            argAux[i][k] = 0;
        }
        commands[0] = getCommand(pipe, arguments);
        commands[1] = getCommand(argsCount - pipe - 1, argAux);
    }
    else {
        commands[0] = getCommand(argsCount, arguments);
    }
    return (pipe > 0) ? 2 : 1;
}

Command getCommand(int argsCount, char args[MAX_ARGS][BUFF_SIZE]) {
    Command toReturn;
    toReturn.p_name = NULL;
    for (int i = 0; i < MAX_ARGS + 1; i++) {
        toReturn.args[i] = NULL;
    }
    for (int i = 0; i < 3; i++) {
        toReturn.fds[i] = fg_fds[i];
    }
    toReturn.function = NULL;
    toReturn.errMsg = NULL;

    if (argsCount <= 0) {
        toReturn.p_name = "N";
        return toReturn;
    }

    if (strCmp(args[0], C_HELP) == 0) {
        toReturn.p_name = "help";
        toReturn.function = help;
        if (argsCount >= 2 && strCmp(args[1], "&") != 0) {
            if (strCmp(args[1], "math") == 0 || strCmp(args[1], "test") == 0) { // || other options)
                toReturn.args[0] = args[1];
            }
            else {
                toReturn.function = NULL;
                toReturn.errMsg = "\nbad request. valid options: 'math', 'test'";
                return toReturn;
            }
        }
    }
    else if (strCmp(args[0], C_FUN) == 0) {
        toReturn.p_name = "fun";
        toReturn.function = (Main)make_invalid_opCode;
    }
    else if (strCmp(args[0], C_CLEAR) == 0) {
        toReturn.function = (Main)clear;
        toReturn.p_name = "clear";
    }
    else if (strCmp(args[0], C_MATH) == 0) {
        toReturn.p_name = "math";
        if (argsCount >= 4) {
            toReturn.function = mathWrapper;
            toReturn.args[0] = args[1];
            toReturn.args[1] = args[2];
            toReturn.args[2] = args[3];
        }
        else {
            toReturn.errMsg = "\nBad arguments. Signature: math op l_val r_val";
            return toReturn;
        }
    }
    else if (strCmp(args[0], C_TIME) == 0) {
        toReturn.function = time;
        toReturn.p_name = "cpu time";
    }
    else if (strCmp(args[0], C_PS) == 0) {
        toReturn.function = (Main)sys_ps;
        toReturn.p_name = "ps";
    }
    else if (strCmp(args[0], C_KILL) == 0) {
        toReturn.function = killWrapper;
        toReturn.p_name = "kill";
        if (argsCount >= 2) {
            toReturn.args[0] = args[1];
        }
        else {
            toReturn.function = NULL;
            toReturn.errMsg = "\nbad args. Signature: kill pid";
            return toReturn;
        }
    }
    else if (strCmp(args[0], C_NICE) == 0) {
        toReturn.function = niceWrapper;
        toReturn.p_name = "nice";
        if (argsCount >= 3) {
            toReturn.args[0] = args[1];
            toReturn.args[1] = args[2];
        }
        else {
            toReturn.function = NULL;
            toReturn.errMsg = "\nbad args. Signature: nice pid priority";
            return toReturn;
        }
    }
    else if (strCmp(args[0], C_BLOCK) == 0) {
        toReturn.function = blockWrapper;
        toReturn.p_name = "block";
        if (argsCount >= 3 && (strCmp(args[2], "0") == 0 || strCmp(args[2], "1") == 0)) {
            toReturn.args[0] = args[1];
            toReturn.args[1] = args[2];
        }
        else {
            toReturn.function = NULL;
            toReturn.errMsg = "\nbad args. Signature: block pid opt\n(opt = 1 : block, opt = 0 : unblock)";
            return toReturn;
        }
    }
    else if (strCmp(args[0], C_FILTER) == 0) {
        toReturn.function = filter;
        toReturn.p_name = "filter";
    }
    else if (strCmp(args[0], C_WC) == 0) {
        toReturn.function = wc;
        toReturn.p_name = "wc";
    }
    else if (strCmp(args[0], C_MEM) == 0) {
        toReturn.function = (Main) sys_memInfo;
        toReturn.p_name = "mem_info";
    }
    else if (strCmp(args[0], C_LOOP) == 0) {
        toReturn.function = loop;
        toReturn.p_name = "loop";
        if (argsCount >= 2) {
            int m_requested;
            if (!strToInt(args[1], &m_requested) || m_requested <= 0) {
                toReturn.function = NULL;
                toReturn.errMsg = "\nbad request. millis_to_wait needs to be a positive number";
                return toReturn;
            }
            toReturn.args[0] = args[1];
        }
        else {
            toReturn.function = NULL;
            toReturn.errMsg = "\nbad args. Signature: loop millis_to_wait";
            return toReturn;
        }
    }
    else if (strCmp(args[0], C_CAT) == 0) {
        toReturn.function = cat;
        toReturn.p_name = "cat";
    }
    else if (strCmp(args[0], C_PHYLO) == 0) {
        toReturn.function=(Main)initPhilosopher;
        toReturn.p_name="philosophers";
    }
    else if (strCmp(args[0], C_TEST) == 0 && argsCount > 1) {
        if (strCmp(args[1], "mm") == 0) {
            if (argsCount >= 3) {
                int m_requested;
                strToInt(args[2], &m_requested);
                if (m_requested > 0 && m_requested <= 128) {
                    toReturn.args[0] = args[2];
                    toReturn.p_name = "test_mm";
                    toReturn.function = (Main)test_mm;
                }
                else {
                    toReturn.errMsg = "\nbad request. min: 1MB, max: 128MB";
                    return toReturn;
                }
            }
            else {
                toReturn.errMsg = "\nbad arguments. Signature: test mm qty";
                return toReturn;
            }
        }
        else if (strCmp(args[1], "processes") == 0) {
            if (argsCount >= 3) {
                int p_requested;
                strToInt(args[2], &p_requested);
                if (p_requested > 0 && p_requested <= 100) {
                    toReturn.args[0] = args[2];
                    toReturn.p_name = "test_processes";
                    toReturn.function = (Main)test_processes;
                }
                else {
                    toReturn.errMsg = "\nbad request. min: 1, max: 100";
                    return toReturn;
                }
            }
            else {
                toReturn.errMsg = "\nbad arguments. Signature: test processes qty";
                return toReturn;
            }
        }
        else if (strCmp(args[1], "priority") == 0) {
            toReturn.function = (Main)test_prio;
            toReturn.p_name = "test_priority";
        }
        else if (strCmp(args[1], "sync") == 0) {
            if (argsCount >= 4) {
                toReturn.function = (Main)test_sync;
                toReturn.p_name = "test_sync";
                toReturn.args[0] = args[2];
                toReturn.args[1] = args[3];
            }
            else {
                toReturn.errMsg = "\nbad arguments. Signature: test sync inc useSem?";
                return toReturn;
            }
        }
        else {
            toReturn.errMsg = "\nbad arguments. \nSignature: test (options: mm, processes, priority, sync)";
            return toReturn;
        }
    }
    else {
        toReturn.errMsg = "\nCommand not found";
        return toReturn;
    }

    if (strCmp(args[argsCount - 1], "&") == 0) {
        toReturn.fds[STDIN] = DEV_NULL;
    }

    return toReturn;
}