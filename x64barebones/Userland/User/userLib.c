#include "../User/include/userLib.h"

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base){
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

uint32_t signedIntToBase(signed int value, char * buffer, uint32_t base){
    int digits = 0;
    if(value < 0){
        uint64_t absVal = value*(-1);
        buffer[0] = '-';
        digits = uintToBase(absVal, &(buffer[1]), base) + 1;
    }
    else{
        digits = uintToBase(value, buffer, base);
    }
    return digits;
}

int strCmp(const char* s1,const char* s2){
    int i = 0;
    while (s1[i] && (s1[i] == s2[i])) {
        i++;
    }
    return s1[i] - s2[i];
}

void concat(char * left, char * right, char * target){
    int idx = 0;
    for (int i = 0 ; left[i] != 0; i++, idx++){
        target[idx] = left[i];
    }
    for(int j = 0; right[j] != 0; j++, idx++){
        target[idx] = right[j];
    }
    target[idx] = 0;
}

uint32_t appendStr(char* trgt, char *src, uint32_t old_len) {
    int i = 0;
    while (src[i]) {
        trgt[old_len++] = src[i++];
    }
    return old_len;
}

void copyStr(const char * origin, char * destiny, int n){
    for (int i = 0; i < n && origin[i]; i++){
        destiny[i] = origin[i];
    }
}

int strLen(const char * s){
    int i ;
    for (i = 0; s[i]; i++);
    return i;
}

int isPrefix(const char * s1, const char * s2, int n){
    int cmp = 0;
    for (int i = 0; s1[i] && s2[i] && i < n && cmp == 0; ++i) {
        cmp = s1[i] - s2[i];
    }
    return cmp == 0;
}

void print(char *str, uint32_t fontHexColor, uint32_t fontSize){
    int16_t fds[3];
    sys_get_FDs(fds);
    sys_write(fds[STDOUT],str, strLen(str)+1, fontHexColor, 0x000000, fontSize);
}

void printUpToEOF(char *str, uint32_t fontHexColor, uint32_t fontSize){
    int16_t fds[3];
    sys_get_FDs(fds);
    while(*str != 1){
        sys_write(fds[STDOUT],str, 1, fontHexColor, 0x000000, fontSize);
        str += 1;
    }
}

void printErr(char *str, uint32_t fontSize){
    int16_t fds[3];
    sys_get_FDs(fds);
    sys_write(fds[STDERR],str, strLen(str)+1, 0xff0000, 0x000000, fontSize);
}

char notNumber(char c){
    return c>='0' && c<='9';
}
char strToInt(const char *str,int *result){
    int toReturn = 0;
    int sign = 1;
    int i=0;
    if (str[0] == '-'){
        sign = -1;
        i++;
    }
    while(str[i]!=0){
        if(!notNumber(str[i])){
            return 0;
        }else{
            toReturn = toReturn*10 + str[i++] - '0';
        }
    }
    *result= toReturn*sign;
    return 1;
}

unsigned int concatenate_bits(unsigned char c1, unsigned char c2) {
    unsigned int result = 0;
    for (int i = 0; i < 8; i++) {
        result <<= 1;
        result |= (c1 & (1 << i)) >> i;
    }
    for (int i = 0; i < 8; i++) {
        result <<= 1;
        result |= (c2 & (1 << i)) >> i;
    }
    return result;
}

int getRandSeed(){
    int timeAux[3];
    sys_getCpuTime(timeAux);
    unsigned int startState = concatenate_bits(timeAux[0],timeAux[1]);
    for (int i = 0; i < 8; i++) {
        startState <<= 1;
        startState |= (timeAux[2] & (1 << i)) >> i;
    }

    int lfsr = startState;

    for (int i = 0; i < (startState & 0xFF); ++i) {
        unsigned lsb = lfsr & 1u;
        lfsr >>= 1;
        if (lsb)
            lfsr ^= 0xE10000;
    }
    lfsr = lfsr%1000000;
    return (lfsr < 0)? lfsr*(-1) : lfsr;
}

int getRandFromRange (int start, int end){
    int rand = getRandSeed();
    return (int)((((double)rand/(double)1000000)*(end-start))+start);
}

int rgbToInt(char red, char green, char blue){
    int color = concatenate_bits(red, green);
    for (int i = 0; i < 8; i++) {
        color <<= 1;
        color |= (blue & (1 << i)) >> i;
    }
    return color;
}