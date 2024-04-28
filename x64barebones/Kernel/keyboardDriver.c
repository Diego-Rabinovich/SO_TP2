#include "include/keyboardDriver.h"

#define R_SHIFT_PRESS 0x36
#define L_SHIFT_PRESS 0x2A
#define R_SHIFT_RELEASE 0xB6
#define L_SHIFT_RELEASE 0xAA
#define F1 0xAA
#define CAPS_LOCK 0x3A
#define BUFF_SIZE 64

char getKeyPress();

char caps_enabled = 0;
char shift_enabled = 0;

unsigned char buffer[BUFF_SIZE] = {0};
unsigned char readIdx = 0;
unsigned char writeIdx = 0;
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

void setKeyFlags(){
    if(buffer[writeIdx] == CAPS_LOCK){
        caps_enabled = !caps_enabled;
    }
    else if (buffer[writeIdx] == R_SHIFT_PRESS || buffer[writeIdx] == L_SHIFT_PRESS){
        shift_enabled = 1;
    }
    else if(buffer[writeIdx] == R_SHIFT_RELEASE || buffer[writeIdx] == L_SHIFT_RELEASE){
        shift_enabled = 0;
    }
}

void keyboardHandler(){
    buffer[writeIdx] = getKeyPress();
    setKeyFlags();
    writeIdx = (writeIdx+1)%BUFF_SIZE;
}

void alignPointers(){
    readIdx=writeIdx;
}

char next(){
    unsigned char toReturn = buffer[readIdx];
    readIdx = (readIdx + 1)%BUFF_SIZE;
    return toReturn;
}

char hasNext(){
    return readIdx != writeIdx;
}

