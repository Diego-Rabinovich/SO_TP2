#include "include/videoDriver.h"
#include "include/keyboardDriver.h"
#include "include/interrupts.h"


#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE 6

void handler();
static void zero_division();
static void invalid_opcode();
extern void printRegs();
extern void returnToUserLand();

void exceptionDispatcher(int exception) {
    resetScreen();
    switch (exception) {
        case ZERO_EXCEPTION_ID:
            zero_division();
            break;
        case INVALID_OPCODE:
            invalid_opcode();
            break;
    }
    printRegs();
    handler();
}

static void zero_division() {
    drawStringWithColor("\nException by zero division\n",28, 0x00FF0000,0, 2);
}

static void invalid_opcode(){
    drawStringWithColor("\nException by invalid opcode\n",29, 0x00FF0000,0, 2);
}
void handler(){
    drawString("\nPress esc to restart\n",25, 2);
    char g=0;
    alignPointers();
    do {
        _hlt();
        if(hasNext()){
            g=next();
        }
    } while (g!=1);
}