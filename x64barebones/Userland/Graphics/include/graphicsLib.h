#ifndef TPEARQUI_GRAPHICSLIB_H
#define TPEARQUI_GRAPHICSLIB_H
#include "../../User/include/userLib.h"
#define CELL_SIZE 32
typedef enum {LEFT,UP,RIGHT,DOWN} Orientation;
Coords getGridSize();
void paintCell(int x, int y, uint32_t hexColor);
void paintCircle(int x, int y, uint32_t hexColor);
void paintTriangle(int x, int y, uint32_t hexColor, Orientation orientation);
void blank(uint32_t hexColor);
#endif //TPEARQUI_GRAPHICSLIB_H
