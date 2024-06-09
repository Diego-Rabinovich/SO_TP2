#include "include/keyboardDriver.h"

#include "interrupts.h"
#include "sysCallDispatcher.h"
#include "include/scheduler.h"
#include "include/videoDriver.h"
#include "include/fileDescriptor.h"

#define R_SHIFT_PRESS 0x36
#define L_SHIFT_PRESS 0x2A
#define R_SHIFT_RELEASE 0xB6
#define L_SHIFT_RELEASE 0xAA
#define L_CTRL_PRESS 0x1D
#define L_CTRL_RELEASE 0x9D

#define F1 0xAA
#define CAPS_LOCK 0x3A
#define BUFF_SIZE 64
char getKeyPress();

char caps_enabled = 0;
char shift_enabled = 0;
char ctrl_enabled = 0;

FileDescriptor stdin_fd;

void initSTDIN(){
    stdin_fd=initFd();
}

unsigned char scan_codes[][84] = {
        {//SIN SHIFT
                NOT_USABLE, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
                '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
                CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', SHIFT,
                '*', ALT, ' ', CAPS, NOT_USABLE, NOT_USABLE, NOT_USABLE,
                NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE,
                NOT_USABLE, NOT_USABLE, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
        },
        {//CON SHIFT
                NOT_USABLE, ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
                '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
                CTRL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
                SHIFT, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', SHIFT,
                '*', ALT, ' ', CAPS, NOT_USABLE, NOT_USABLE, NOT_USABLE,
                NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE, NOT_USABLE,
                NOT_USABLE, NOT_USABLE, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
        }
};

unsigned char getStringFromCode(unsigned char code){
    unsigned char alternate = checkAlternate(code);
    if(code&RELEASE){
        code-=RELEASE;
    }
    return scan_codes[alternate][code];
}

unsigned char checkAlternate(unsigned char code) {
    unsigned char alternate = 0;
    char isLetter = (code >= 0x10 && code <= 0x19) || (code >= 0x1E && code <= 0x26) || (code >= 0x2C && code <= 0x32);
    alternate = (isLetter && caps_enabled);
    if(shift_enabled) {
        alternate = !alternate;
    }
    return alternate;
}

void setKeyFlags(unsigned char key){
    if(key == CAPS_LOCK){
        caps_enabled = !caps_enabled;
    }
    else if (key == R_SHIFT_PRESS || key == L_SHIFT_PRESS){
        shift_enabled = 1;
    }
    else if(key== R_SHIFT_RELEASE || key == L_SHIFT_RELEASE){
        shift_enabled = 0;
    }
    else if(key== L_CTRL_PRESS){
        ctrl_enabled = 1;
    }
    else if(key == L_CTRL_RELEASE){
        ctrl_enabled = 0;
    }
}

void keyboardHandler(){
    _cli();
    // drawStringWithColor(" START_K ", 14, 0xffff00, 0, 2);
    char key[1]={getKeyPress()};
    if(key[0] == 0x2C){
        //APRIETO Z
        ps();
    }
    writeOnFile( stdin_fd,key,1, 0x000000, 0x000000, 2);
    setKeyFlags(key[0]);
    if(ctrl_enabled && key[0] == 0x2E){
        killFG();
        drawString("\n$>", 3, 2);
    }
    // drawStringWithColor(" END-K ", 14, 0xffff00, 0, 2);
}






//LA HISTORIA DE AMOR
//void alignPointers(){
//    readIdx=writeIdx;
//}
