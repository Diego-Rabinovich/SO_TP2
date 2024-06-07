/* shell.c */
#include "../User/include/userLib.h"
#include "include/shellLib.h"
#include "include/shellMain.h"
#include "include/commands.h"
#include "../Snake/include/snakeMain.h"
#include "../Math/include/math.h"
#include "../Testing/testing.h"


char buffer[BUFF_SIZE] = {0};
char  arguments[MAX_ARGS][128];
int idx = 0;
int fontSize = 2;
int fontColor = 0xffffff;
const int shellFontColor = 0xffffff;
Coords dims;

void clear();
char startCommand();

void header(){
    print("Welcome to PEPE OS\n", shellFontColor, fontSize);
    newLine(fontSize);
}

void resetBuffer() {
    for (int i = 0; i < BUFF_SIZE; i++) {
        buffer[i] = 0;
    }
    idx = 0;
}


int shell(int argc, char **args) {
    sys_getScreenDimensions(&dims);
    clear();
    while (1) {
        int size = sys_read(0, buffer + idx, 1);
        if (size > 0 && !(buffer[idx] & 0x80)) {
            switch (buffer[idx]) {
                case '\n': {
                    char f = startCommand();
                    if (f == 0){
                        newLine(fontSize);
                    }else if (f == 1){
                        clear();
                    }
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
                        print(buffer + idx - 1, 0, fontSize);
                    }
                    break;
                default:
                    if (idx + 1 < BUFF_SIZE && !isSpecial(buffer[idx])) {
                        print(buffer + idx, fontColor, fontSize);
                        idx++;
                    } else if (idx + 1 < BUFF_SIZE) {
                        buffer[idx] = 0; //borro las especiales que no nos importan aquí
                    }
                    break;
            }
        }
    }
}


char startCommand() {
    int argsCount = parseArgs(buffer, arguments, MAX_ARGS, ' ', '\n');

    if (argsCount <= 0){
        return 0;
    }

    if (strCmp(arguments[0], C_SET) == 0 && argsCount > 1) {

        if (strCmp(arguments[1], "font_size") == 0 && argsCount == 3) { //caso font size (exactamente 3 args)
            int size = 0;
            if(strToInt(arguments[2], &size)> 0 && size > 0 && size <= 10){
                print("\n", shellFontColor, fontSize);
                fontSize = size;
                print("$>", shellFontColor, fontSize);
                return 1;
            } else if(size <= 0 || size > 10 ){
                sys_write(2, "ERROR: The font size can only take values between 1 and 10",58, 0, fontSize);
            }
        }

        else if (strCmp(arguments[1], "font_color") == 0 && argsCount == 5){
            int red, green, blue;
            if (strToInt(arguments[2], &red) + strToInt(arguments[3], &green) + strToInt(arguments[4], &blue) == 3){
                if (red >= 0 && red < 256 && green >= 0 && green < 256 && blue >= 0 && blue < 256){
                    fontColor = rgbToInt((char)red, (char)green, (char)blue);
                } else{
                    sys_write(2, "\nbad RGB arguments", 19,0, fontSize);
                }
            }
            else {
                commandNotFound(fontSize);
            }
        }

        else {
            commandNotFound(fontSize);
        }
    }


    else if (strCmp(arguments[0], C_HELP) == 0) {

        if (argsCount == 1) { //general help
            print("\nHelp Menu:\n", shellFontColor, fontSize);
            print("To execute a command write the command and press enter\n", shellFontColor, fontSize);
            print("(opt:type1) means that argument is of type type1, and it is optional\n", shellFontColor, fontSize);
            print("(type1) means that argument is of type type1, and the argument must appear in the call\n", shellFontColor, fontSize);
            print("(opt:type) ... means that command accepts different numbers of arguments, and so they are always optionals\n\n", shellFontColor, fontSize);
            print("ID - SYNTAX - ARGUMENTS - DESCRIPTION \n\n", 0x000000ff, fontSize);
            print("1 - help", 0x000000ff, fontSize);
            print(" (opt:command) arg1  - provides info about the arg1 or about the shell when called without arguments\n\n", shellFontColor, fontSize);
            print("2 - set", 0x000000ff, fontSize);
            print(" (property) arg1, (opt:value) ...  - sets the arg1 with the values of ar2, arg3... \n\n", shellFontColor, fontSize);
            print("3 - math", 0x000000ff, fontSize);
            print(" - (binary operation) op, (val) left, (val) right - aplies the op to left and right \n\n", shellFontColor, fontSize);
            print("4 - snake", 0x000000ff, fontSize);
            print(" - (integer) arg1 - opens Snake game with the arg1 gamemode\n\n", shellFontColor, fontSize);
            print("5 - clear", 0x000000ff, fontSize);
            print("- NONE - reset shell\n\n", shellFontColor, fontSize);
            print("6 - fun", 0x000000ff, fontSize);
            print(" - NONE - throws an invalid opCode exception, just for fun\n", shellFontColor, fontSize);
            print("7 - time", 0x000000ff, fontSize);
            print(" - NONE - prints the current local time\n", shellFontColor, fontSize);
        }

        else if (argsCount == 2){

            if (strCmp(arguments[1], "snake")==0){ //case snake
                print("\nHelp Menu: Snake Game\n", shellFontColor, fontSize);
                print("Solo mode: 1. Call: snake 1\n", shellFontColor, fontSize);
                print("Duo mode: 2. Call: snake 2\n\n", shellFontColor, fontSize);
            }

            else if(strCmp(arguments[1], "set")==0){//case set
                print("\nHelp Menu: Set\n", shellFontColor, fontSize);
                print("Properties list:\n\n", shellFontColor, fontSize);
                print("- font_size. Args: (integer) size \n", shellFontColor, fontSize);
                print("- font_color. Args: (integer:0-255) red, (integer:0-255) green, (integer:0-255) blue\n", shellFontColor, fontSize);
                print("\n", shellFontColor, fontSize);
                print("Call example: set font_color 120 155 100\n", shellFontColor, fontSize);
            }

            else if(strCmp(arguments[1], "math") == 0){
                print("\n Help Menu: Math\n\n", shellFontColor, fontSize);
                print("Use:", shellFontColor, fontSize);
                print(" ", shellFontColor, fontSize);
                print("ans", 0x12345678, fontSize);
                print(" as left or/and right to operate with the last calculated result\n\n", shellFontColor, fontSize);
                print("List of valid operations: \n", shellFontColor, fontSize);
                print("mul", 0x000000ff, fontSize);
                print(" - ex: math mul 1 3, returns 1*3=3\n", shellFontColor, fontSize);
                print("sub", 0x000000ff, fontSize);
                print(" - ex: math sub ans 3, returns (if ans = 3): 3-3=0\n", shellFontColor, fontSize);
                print("sum", 0x000000ff, fontSize);
                print(" - ex: math sum 1 3, returns 1+3=4\n", shellFontColor, fontSize);
                print("div", 0x000000ff, fontSize);
                print(" - ex: math div 3 3, returns 3/3=1. Note: div x 0 will throw exception\n", shellFontColor, fontSize);
                print("pot", 0x000000ff, fontSize);
                print(" - ex: math pot 1 3, returns 2^3=8\n", shellFontColor, fontSize);
                print("mod", 0x000000ff, fontSize);
                print(" - ex: math mod 10 5, returns 10%5=0\n", shellFontColor, fontSize);
            }

        }
        else {
            commandNotFound(fontSize);
        }
    }

    else if(strCmp(arguments[0], C_SNAKE) == 0){
        if (argsCount != 2) {
            sys_write(2, "\nERROR: Se debe senalar la cantidad de jugadores a jugar (1 o 2) como unico argumento\n", 88, 0, fontSize);
            return 0;
        } else {
            int players;
            strToInt(arguments[1], &players);
            switch (players) {
                case 1:
                    sys_blankArea(0, dims.x, 0, dims.y, 0);
                    sys_resetCursor();
                    snake(SINGLE);
                    break;
                case 2:
                    sys_blankArea(0, dims.x, 0, dims.y, 0);
                    sys_resetCursor();
                    snake(DUOS);
                    break;
                default:
                    sys_write(2, "ERROR: Cantidad de jugadores erronea", 36, 0, fontSize);
                    return 0;
            }
            return 1;
        }
    }

    else if (strCmp(arguments[0], C_FUN) == 0 && argsCount == 1){
        make_invalid_opCode();
        return 0;
    }

    else if (strCmp(arguments[0], C_CLEAR) == 0 && argsCount == 1){
        clear();
        return 2;
    }

    else if (strCmp(arguments[0], C_MATH) == 0 && argsCount == 4){
        signed int res = 0;
        if(math(arguments[1], arguments[2], arguments[3], &res)){
            char str[32];
            signedIntToBase(res, str, 10);
            print("\nResult: ", shellFontColor, fontSize);
            print(str, shellFontColor, fontSize);
        }
        else{
            sys_write(2, "\nThere was an error", 19, 0x00FF0000, fontSize);
        }
        return 0;
    }

    else if (strCmp(arguments[0], C_TIME) == 0 && argsCount == 1){
        int time[3];
        sys_getCpuTime(time);

        newLine(fontSize);
        char str[3];
        uintToBase(time[0], str, 10);
        print(str, shellFontColor, fontSize);
        print(":", shellFontColor, fontSize);
        uintToBase(time[1], str, 10);
        print(str, shellFontColor, fontSize);
        print(":", shellFontColor, fontSize);
        uintToBase(time[2], str, 10);
        print(str, shellFontColor, fontSize);

        return 0;
    }
    else if(strCmp(arguments[0], C_MEM) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0],C_PS ) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_LOOP) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_KILL) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_NICE) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_BLOCK) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_CAT) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_WC) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0],C_FILTER ) == 0 ){
        return 0;
    }
    else if(strCmp(arguments[0], C_PHYLO) == 0 ){
        return 0;
    }

    else if (strCmp(arguments[0], C_TEST) == 0 && argsCount > 1){
        if (strCmp(arguments[1], "mm") == 0 && argsCount > 2){
            int m_requested;
            strToInt(arguments[2], &m_requested);
            if (m_requested > 0 && m_requested <= 128){
                char ** args= sys_malloc(sizeof (char *)*2+10);
                args[0]=(char*)args+ sizeof(char*)*2;
                uintToBase(m_requested*1024*1024, args[0], 10);
                print("\nnow allocating: ", 0xffffff, 2);
                print(args[0], 0xffffff, 2);
                char *argsAux[2]={args[0],0};
                int16_t pid=sys_fork((Main) test_mm,argsAux,"test_mm",3);
                sys_wait_pid(pid);
                sys_free(args);
            } else {
                print("\nThe requested memory space must be between 1MB and 128MB", 0xff0000, 2);
            }
        }
        else if (strCmp(arguments[1], "processes") == 0 && argsCount > 2) {
            int p_requested;
            strToInt(arguments[2], &p_requested);
            if (p_requested > 0 && p_requested <= 4000) {
                char * argsAux[2] = {arguments[2],0};
                int16_t pid=sys_fork((Main) test_processes,argsAux,"test_ps",3);
                sys_wait_pid(pid);
            } else {
                print("\nThe requested max processes must be between 1 and 4000", 0xff0000, 2);
            }
        }
        else if (strCmp(arguments[1], "priority") == 0) {
            char * argsNull[]={0};
            int16_t pid=sys_fork((Main)test_prio,argsNull,"test_prio",3);
            sys_wait_pid(pid);
        }
        else {
            print("\nBad arguments, run <help test> for more info", 0xff0000, 2);
        }
    }

    else {
        commandNotFound(fontSize);
    }
    return 0;
}

void clear(){
    sys_blankArea(0,dims.x, 0, dims.y, 0);
    sys_resetCursor();
    header();
}
