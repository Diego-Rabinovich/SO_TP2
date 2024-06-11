// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <console.h>


static char buffer[64] = { '0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0' };
static uint16_t * const video = (uint16_t*)0xB8000;
static uint16_t * currentVideo = (uint16_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;

void print(const char * string)
{
    int i;

    for (i = 0; string[i] != 0; i++){
        printChar(string[i]);
    }
}

uint16_t formatChar(unsigned char c, unsigned char forecolour, unsigned char backcolour) {
    return  (c | ((backcolour << 4) | (forecolour & 0x0F)) << 8);
}

void printCharWithColor(uint16_t fchar){
    *currentVideo = fchar;
    currentVideo += 1;
}

void printWithColor(char * string, unsigned char forecolour, unsigned char backcolour){
    int i;
    for (i = 0; string[i] != 0; i++){
        printCharWithColor(formatChar(string[i], forecolour, backcolour));
    }
}

void printChar(char character)
{
    printCharWithColor(formatChar(character, 0x0F, 0x00));
}

void newLine()
{
    do
    {
        printChar(' ');
    }
    while((uint64_t)(currentVideo - video) % (width) != 0);
}

void printDec(uint64_t value)
{
    printBase(value, 10);
}

void printHex(uint64_t value)
{
    printBase(value, 16);
}

void printBin(uint64_t value)
{
    printBase(value, 2);
}

void printBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    print(buffer);
}

void clear()
{
    int i;

    for (i = 0; i < height * width; i++)
        video[i * 2] = ' ';
    currentVideo = video;
}

