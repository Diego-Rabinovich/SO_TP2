#include "include/lib.h"

#include "fileDescriptor.h"
#include "scheduler.h"
#include "include/videoDriver.h"
#include "include/keyboardDriver.h"
#include "include/interrupts.h"

uint64_t* dumpRegs();
char stored();

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    //Calculate characters for each digit
    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    // Terminate string in buffer.
    *p = 0;

    //Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

void * memset(void * destination, int32_t c, uint64_t length)
{
    uint8_t chr = (uint8_t)c;
    char * dst = (char*)destination;

    while(length--)
        dst[length] = chr;

    return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
    /*
    * memcpy does not support overlapping buffers, so always do it
    * forwards. (Don't change this without adjusting memmove.)
    *
    * For speedy copying, optimize the common case where both pointers
    * and the length are word-aligned, and copy word-at-a-time instead
    * of byte-at-a-time. Otherwise, copy by bytes.
    *
    * The alignment logic below should be portable. We rely on
    * the compiler to be reasonably intelligent about optimizing
    * the divides and modulos out. Fortunately, it is.
    */
    uint64_t i;

    if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
        (uint64_t)source % sizeof(uint32_t) == 0 &&
        length % sizeof(uint32_t) == 0)
    {
        uint32_t *d = (uint32_t *) destination;
        const uint32_t *s = (const uint32_t *)source;

        for (i = 0; i < length / sizeof(uint32_t); i++)
            d[i] = s[i];
    }
    else
    {
        uint8_t * d = (uint8_t*)destination;
        const uint8_t * s = (const uint8_t*)source;

        for (i = 0; i < length; i++)
            d[i] = s[i];
    }

    return destination;
}

int getFormat(int num) {
    int dec = num & 240;
    dec = dec >> 4;
    int units = num & 15;
    return dec * 10 + units;
}

void getCurrentCpuTime(int array[3]){
    int* aux = cpuGetTime();
    array[0] = (getFormat(aux[0])+21) % 24;
    array[1] = getFormat(aux[1]);
    array[2] = getFormat(aux[2]);
}

int waitNMillis(const uint64_t millis){
    const uint64_t limit = millis_elapsed() + millis;
    _sti();
    while (millis_elapsed() < limit) {}
    return limit;
}


char shownRegisters = 0;

void printRegs(){
    uint64_t* sRegisters;
    sRegisters = dumpRegs();

    char str[32];

    int idx = 0;
    drawString("RDI: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("RSI: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);


    drawString("RAX: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("RBX: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R8:  ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R9:  ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R10: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R11: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R12: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R13: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R14: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("R15: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("RIP: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("RSP: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);

    drawString("RBP: ", 5, 2);
    uintToBase(sRegisters[idx++], str, 16);
    drawString(str, strLen(str), 2);
    drawString("\n", 1, 2);
}

int strLen(const char * s){
    int i ;
    for (i = 0; s[i]; i++);
    return i;
}

int arrayLen(void **array){
    int len = 0;
    while (*(array++) != 0x00000000){
        len++;
    }
    return len;
}

int strCmp(const char* s1,const char* s2){
    int i = 0;
    while (s1[i] && (s1[i] == s2[i])) {
        i++;
    }
    return s1[i] - s2[i];
}
