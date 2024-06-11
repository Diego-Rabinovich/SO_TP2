#ifndef TPEARQUI_USERLIB_H
#define TPEARQUI_USERLIB_H
#include "userLibAsm.h"
char strToInt(const char *str,int *result);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
uint32_t signedIntToBase(signed int value, char * buffer, uint32_t base);
int strCmp(const char* s1,const char* s2);
void copyStr(const char * origin, char * destiny, int n);
int strLen(const char * s);
int isPrefix(const char * s1, const char * s2, int n);
int getRandFromRange (int start, int end);
void print(char* str, uint32_t fontHexColor, uint32_t fontSize);
unsigned int concatenate_bits(unsigned char c1, unsigned char c2);
int rgbToInt(char red, char green, char blue);
typedef int (*Main)(int argc, char **args);
void concat(char * left, char * right, char * target);
void printErr(char *str, uint32_t fontSize);
void printUpToEOF(char *str, uint32_t fontHexColor, uint32_t fontSize);
uint32_t appendStr(char* trgt, char *src, uint32_t old_len);

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

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define DEV_NULL -1

#endif //TPEARQUI_USERLIB_H
