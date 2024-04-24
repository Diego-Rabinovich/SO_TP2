#include "stdint.h"
#include "include/audioDriver.h"
#include "include/lib.h"
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

static void play_sound(uint32_t nFrequence) {
    uint32_t Div;
    uint8_t tmp;

    //Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (Div) );
    outb(0x42, (uint8_t) (Div >> 8));

    //And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}


//make it shutup
static void nosound() {
    uint8_t tmp = inb(0x61) & 0xFC;

    outb(0x61, tmp);
}


//Make a beep
void beep(uint32_t frequency, uint64_t duration) {
    play_sound(frequency);
    waitNMillis(duration);
    nosound();
}