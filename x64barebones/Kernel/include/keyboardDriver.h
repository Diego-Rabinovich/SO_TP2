#ifndef TPEARQUI_KEYBOARDDRIVER_H
#define TPEARQUI_KEYBOARDDRIVER_H

#define E_KEYS 0xE0
#define GREY_UP_SCAN 0x48
#define GREY_DOWN_SCAN 0x50
#define GREY_LEFT_SCAN 0x4B
#define GREY_RIGHT_SCAN 0x4D
#define R_CTRL_SCAN 0x1D
#define R_ALT_SCAN 0x38
#define RELEASE 0x80

#define NOT_USABLE 0

#define ESC 1
#define CTRL 2
#define SHIFT 3
#define CAPS 4
#define ALT 5

#define LEFT_ARROW 10
#define RIGHT_ARROW 11
#define UP_ARROW 12
#define DOWN_ARROW 13


unsigned char getStringFromCode(unsigned char code);
char checkAlternate(unsigned char code);
void keyboardHandler();
void alignPointers();
char hasNext();
char next();
#endif //TPEARQUI_KEYBOARDDRIVER_H
