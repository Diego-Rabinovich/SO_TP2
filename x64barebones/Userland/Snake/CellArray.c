#include "include/CellArray.h"

void cpyCell(Cell* target, Cell src){
    target->x = src.x;
    target->y = src.y;
    target->cellType = src.cellType;
}

void updateHead(CellArray *  cellArray, Cell head){

    cellArray->head = (cellArray->head + 1) % CELLS_QTY;

    cpyCell(&((cellArray->array)[cellArray->head]),head);
}

void updateTail(CellArray *  cellArray){
    cellArray->tail = (cellArray->tail + 1) % CELLS_QTY;
}

char isEmpty(const CellArray array){
    return array.head==array.tail;
}

int cellEquals(Cell c1, Cell c2){
    return c1.x == c2.x && c1.y == c2.y && c1.cellType == c2.cellType;
}
