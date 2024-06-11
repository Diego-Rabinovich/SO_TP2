/* shell.c */
#include "../User/include/userLib.h"
#include "include/shellLib.h"
#include "include/shellMain.h"
#include "include/commands.h"
#include "../Math/include/math.h"
#include "../Testing/testing.h"

#define CTRL_PRESS 2
#define CTRL_RELEASE 0x82
#define NULL 0x00000000
#define EOF 0

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

int filter(int argc, char** argv);
int help(int argc, char** argsv);
int mathWrapper(int argc, char** argv);
int time(int argc, char** argv);
int killWrapper(int argc, char** argv);
int blockWrapper(int argc, char** argv);
int niceWrapper(int argc, char** argv);
int cat(int argc, char** argv);
int wc(int argc, char** argv);
int loop(int argc, char** argv);
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
        commands[0] = getCommand(pipe, arguments);
        commands[1] = getCommand(argsCount - pipe - 1, arguments + pipe + 1);
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
        if (argsCount >= 2) {
            if (strCmp(args[1], "math") == 0) { // || other options)
                toReturn.args[0] = args[1];
            }
            else {
                toReturn.function = NULL;
                toReturn.errMsg = "bad request. valid options: 'math'";
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
            toReturn.function = NULL;
            toReturn.errMsg = "Bad arguments. Signature: math op l_val r_val";
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
            toReturn.errMsg = "bad args. Signature: kill pid";
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
            toReturn.errMsg = "bad args. Signature: nice pid priority";
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
            toReturn.errMsg = "bad args. Signature: block pid opt\n(opt = 1 : block, opt = 0 : unblock)";
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
                toReturn.errMsg = "bad request. millis_to_wait needs to be a positive number";
                return toReturn;
            }
            toReturn.args[0] = args[1];
        }
        else {
            toReturn.function = NULL;
            toReturn.errMsg = "bad args. Signature: loop millis_to_wait";
            return toReturn;
        }
    }
    else if (strCmp(args[0], C_CAT) == 0) {
        toReturn.function = cat;
        toReturn.p_name = "cat";
    }
    else if (strCmp(args[0], C_PHYLO) == 0) {}
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
                    toReturn.function = NULL;
                    toReturn.errMsg = "bad request. min: 1MB, max: 128MB";
                    return toReturn;
                }
            }
            else {
                toReturn.function = NULL;
                toReturn.errMsg = "bad arguments. Signature: test mm qty";
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
                    toReturn.function = NULL;
                    toReturn.errMsg = "bad request. min: 1, max: 100";
                    return toReturn;
                }
            }
            else {
                toReturn.function = NULL;
                toReturn.errMsg = "bad arguments. Signature: test processes qty";
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
                toReturn.function = NULL;
                toReturn.errMsg = "bad arguments. Signature: test sync inc useSem?";
                return toReturn;
            }
        }
    }
    else {
        toReturn.function = NULL;
        toReturn.errMsg = "Comand not found";
        return toReturn;
    }

    if (strCmp(args[argsCount - 1], "&") == 0) {
        toReturn.fds[STDIN] = DEV_NULL;
    }

    return toReturn;
}

int help(int argc, char** argsv) {
    char buff[1024] = {0};
    uint32_t len = 0;
    if (argc == 0) { //general help
        len = appendStr(buff, "Help Menu:\n", len);
        len = appendStr(buff, "To execute a command write the command and press enter\n", len);
        len = appendStr(buff, "(opt:type1) means that argument is of type type1, and it is optional\n", len);
        len = appendStr(
            buff, "(type1) means that argument is of type type1, and the argument must appear in the call\n", len);
        len = appendStr(
            buff,
            "(opt:type) ... means that command accepts different numbers of arguments, and so they are always optionals\n\n",
            len);
        len = appendStr(buff, "ID - SYNTAX - ARGUMENTS - DESCRIPTION \n\n", len);
        len = appendStr(buff, "1 - help", len);
        len = appendStr(
            buff,
            " (opt:command) arg1  - provides info about the arg1 or about the shell when called without arguments\n\n",
            len);
        len = appendStr(buff, "2 - math", len);
        len = appendStr(
            buff, " - (binary operation) op, (val) left, (val) right - aplies the op to left and right \n\n", len);
        len = appendStr(buff, "3 - clear", len);
        len = appendStr(buff, "- NONE - reset shell\n\n", len);
        len = appendStr(buff, "4 - fun", len);
        len = appendStr(buff, " - NONE - throws an invalid opCode exception, just for fun\n", len);
        len = appendStr(buff, "5 - time", len);
        len = appendStr(buff, " - NONE - prints the current local time\n", len);
    }
    else if (argc == 1) {
        if (strCmp(argsv[0], "math") == 0) {
            len = appendStr(buff, "Help Menu: Math\n\n", len);
            len = appendStr(buff, "Use: ans as left or/and right to operate with the last calculated result\n\n", len);
            len = appendStr(buff, "List of valid operations: \n", len);
            len = appendStr(buff, "mul - ex: math mul 1 3, returns 1*3=3\n", len);
            len = appendStr(buff, "sub - ex: math sub ans 3, returns (if ans = 3): 3-3=0\n", len);
            len = appendStr(buff, "sum - ex: math sum 1 3, returns 1+3=4\n", len);
            len = appendStr(buff, "div - ex: math div 3 3, returns 3/3=1. Note: div x 0 will throw exception\n", len);
            len = appendStr(buff, "pot - ex: math pot 1 3, returns 2^3=8\n", len);
            len = appendStr(buff, "mod - ex: math mod 10 5, returns 10%5=0\n", len);
        }
    }
    print(buff, 0xffffff, 2);
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
    while ((c = chain[i]) != EOF && i < 1024) {
        if (!(c & 0x80) && c != 'a' && c != 'A' && c != 'e' && c != 'E' && c != 'i' && c != 'I' && c != 'o' && c != 'O'
            && c != 'u' && c != 'U') {
            toReturn[j++] = chain[i];
        }
        i++;
        sys_read(fds[STDIN], chain + i, 1);
    }

    toReturn[j] = '\0';
    print(toReturn, 0xffffff, 2);
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
    while ((c = chain[i]) != EOF && i < 1024) {
        if (!(c & 0x80)) {
            toReturn[j++] = chain[i];
        }
        i++;
        sys_read(fds[STDIN], chain + i, 1);
    }

    toReturn[j] = '\0';
    print(toReturn, 0xffffff, 2);
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
