#ifndef TPEARQUI_CELLARRAY_H
#define TPEARQUI_CELLARRAY_H
#define X_SIZE 30
#define Y_SIZE 20
#define CELLS_QTY ( X_SIZE * Y_SIZE )


typedef enum {NEUTRAL, APPLE, DEATH} CellType;

typedef struct{
    int x;
    int y;
    CellType cellType;
} Cell;

typedef struct {
    Cell array[CELLS_QTY];
    unsigned int head;
    unsigned int tail;
}CellArray;

void cpyCell(Cell* target, Cell src);
void updateHead(CellArray *  array, Cell head);
void updateTail(CellArray *  array);
char isEmpty(const CellArray array);

#endif //TPEARQUI_CELLARRAY_H
