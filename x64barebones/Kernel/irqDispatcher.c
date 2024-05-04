#include "include/time.h"
#include "include/console.h"
#include "include/keyboardDriver.h"


static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq) {
    switch (irq) {
        case 0:
            int_20();
            break;
        case 1:
            int_21();
            break;
    }
    return;
}


void int_20() {
    timer_handler();
    if(ticks_elapsed()%3==0){
        return interesting_handler;
    }
}

void int_21(){
    keyboardHandler();
}
