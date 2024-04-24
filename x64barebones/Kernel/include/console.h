#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void print(const char * string);
void printChar(char character);
void newLine();
void printDec(uint64_t value);
void printHex(uint64_t value);
void printBin(uint64_t value);
void printBase(uint64_t value, uint32_t base);
uint16_t formatChar(unsigned char c, unsigned char forecolour, unsigned char backcolour);
void printCharWithColor(uint16_t fchar);
void printWithColor(char * string, unsigned char forecolour, unsigned char backcolour);
void clear();

#endif