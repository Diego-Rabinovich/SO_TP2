#ifndef TPEARQUI_VIDEODRIVER_H
#define TPEARQUI_VIDEODRIVER_H
#include <stdint.h>
#define X_LIMIT 1008
#define Y_LIMIT 740
#define X_START_OFFSET 20
#define Y_START_OFFSET 20

#define X_FULLSIZE 1024
#define Y_FULLSIZE 768

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void saveScreenState();
void loadScreenState();
uint32_t getPixel(uint64_t x, uint64_t y);
void  drawChar(unsigned  char c, uint32_t fontSize);
void drawCharWithColor( unsigned char c,uint32_t hexColorFont,uint32_t hexColorBackground, uint32_t fontSize);
void drawString(const char * string,uint64_t len, uint32_t fontSize);
void drawStringWithColor(const char *string, uint64_t len,uint32_t hexColorFont,uint32_t hexColorBackground, uint32_t fontSize);
void resetCursor();
void scrollDown(uint32_t fontSize);
int checkLimits(uint32_t fontSize);
void blankArea(int x0, int xf, int y0, int yf,  uint32_t hexColor);
void resetScreen();
#endif //TPEARQUI_VIDEODRIVER_H
