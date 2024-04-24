#include "include/graphicsLib.h"

Coords screenDim = {0, 0};

Coords gridSize = {0,0};
int pixelSize = 0;

char circle[8][8] = {
        {0,0,0,1,1,0,0,0},
        {0,1,1,1,1,1,1,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,1,1,0,0,0},
};

char triangle[8][8] = {
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1}
};

char square[8][8] = {
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1}
};
void checkScreenDims(){
    if(screenDim.x == 0 && screenDim.y == 0){
        sys_getScreenDimensions(&screenDim);
        gridSize.x = screenDim.x / CELL_SIZE;
        gridSize.y = screenDim.y / CELL_SIZE;
        pixelSize = CELL_SIZE/8;
    }
}

void blank(uint32_t hexColor){
    checkScreenDims();
    for (int i = 0; i < screenDim.x; ++i) {
        for (int j = 0; j < screenDim.y; ++j) {
            sys_paintPixel(i, j, hexColor);
        }
    }
}

Coords getGridSize(){
    checkScreenDims();
    return gridSize;
}

void paintMatrix(int x, int y, char matrix[8][8], uint32_t hexColor, Orientation orientation){
    checkScreenDims();
    if(x < gridSize.x && x >= 0 && y < gridSize.y && y >= 0){
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                for (int k = 0; k <= pixelSize; ++k) {
                    for (int l = 0; l <= pixelSize; ++l) {
                        if(matrix[i][j]){
                            switch (orientation) {
                                case UP:
                                    sys_paintPixel(x*CELL_SIZE+j*pixelSize + k,y*CELL_SIZE+i*pixelSize + l, hexColor);
                                    break;
                                case LEFT:
                                    sys_paintPixel(x*CELL_SIZE+i*pixelSize + k,y*CELL_SIZE+j*pixelSize + l, hexColor);
                                    break;
                                case DOWN:
                                    sys_paintPixel(x*CELL_SIZE+(8-j)*pixelSize + k,y*CELL_SIZE+(8-i)*pixelSize + l, hexColor);
                                    break;
                                case RIGHT:
                                    sys_paintPixel(x*CELL_SIZE+(8-i)*pixelSize + k,y*CELL_SIZE+(8-j)*pixelSize + l, hexColor);
                                    break;

                            }
                        }
                    }
                }
            }
        }
    }
}

void paintCell(int x, int y, uint32_t hexColor){
    paintMatrix(x, y, square, hexColor,  UP);
}

void paintCircle(int x, int y, uint32_t hexColor){
    paintMatrix(x, y, circle, hexColor, UP);
}
void paintTriangle(int x, int y, uint32_t hexColor, Orientation orientation){
    paintMatrix(x, y, triangle, hexColor, orientation);
}
