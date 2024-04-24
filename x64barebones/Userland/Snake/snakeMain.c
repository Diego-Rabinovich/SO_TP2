#include "include/snakeMain.h"
#include "../Graphics/include/graphicsLib.h"
#include "./include/CellArray.h"

typedef struct {
    Orientation orientation;
    char playing;
    uint32_t bodyColor;
    uint32_t headColor;
    CellArray cells;
}Snake;

CellType map[Y_SIZE][X_SIZE];
Cell neutralCells[CELLS_QTY];
int neutralCellsSize;
Cell* appleCell;

Snake snake1 = {0}, snake2 = {0};

void spawnApple(Cell* apple) {
    int randIdx = getRandFromRange(0, neutralCellsSize);
    cpyCell(apple, neutralCells[randIdx]);

    neutralCells[randIdx] = neutralCells[--neutralCellsSize];
    map[apple->y][apple->x] = APPLE;
}

void drawCellBackground(int x, int y) {
    if((x+y) % 2){
        paintCell(x,y + 2,BGCOLOR_1);
    }else{
        paintCell(x,y + 2,BGCOLOR_2);
    }
}

void drawEntireBackground(Coords gridSize){
    for(int i=0;i<=gridSize.x;i++){
        for(int j=0;j<= gridSize.y;j++){
            drawCellBackground(i, j);
        }
    }
}

void initializeMap(SNAKE_MODE mode){
    for (int i = 0; i < X_SIZE; ++i) {
        for (int j = 0; j < Y_SIZE; ++j) {
            map[j][i]=NEUTRAL;
            Cell newCell = {i, j, NEUTRAL};
            cpyCell(&(neutralCells[neutralCellsSize++]),newCell);
        }
    }

    snake1.playing=1;
    snake1.orientation=UP;
    snake1.bodyColor=0x0000FF; //azul
    snake1.headColor=0; //negro
    snake1.cells.head=0;
    snake1.cells.tail=0;
    (snake1.cells.array[ snake1.cells.head]).y=18;
    (snake1.cells.array[ snake1.cells.head]).x=10;
    (snake1.cells.array[snake1.cells.head]).cellType=DEATH;
    map[18][10] = DEATH;
    neutralCells[10*Y_SIZE+18] = neutralCells[--neutralCellsSize];
    snake1.cells.head+=1;
    (snake1.cells.array[ snake1.cells.head]).x=10;
    (snake1.cells.array[ snake1.cells.head]).y=17;
    map[17][10] = DEATH;
    neutralCells[10*Y_SIZE+17] = neutralCells[--neutralCellsSize];
    (snake1.cells.array[snake1.cells.head]).cellType=DEATH;

    if(mode==DUOS){
        snake2.bodyColor=0xFF5B00; //naranja
        snake2.headColor=0; //negro
        snake2.playing=1;
        snake2.orientation=UP;
        snake2.cells.head=0;
        snake2.cells.tail=0;
        (snake2.cells.array[ snake2.cells.head]).y=18;
        (snake2.cells.array[ snake2.cells.head]).x=11;
        (snake2.cells.array[snake2.cells.head]).cellType=DEATH;
        map[18][11] = DEATH;
        neutralCells[11*Y_SIZE+18] = neutralCells[--neutralCellsSize];
        snake2.cells.head+=1;
        (snake2.cells.array[ snake2.cells.head]).x=11;
        (snake2.cells.array[ snake2.cells.head]).y=17;
        (snake2.cells.array[snake2.cells.head]).cellType=DEATH;
        map[17][11] = DEATH;
        neutralCells[11*Y_SIZE+17] = neutralCells[--neutralCellsSize];
    }else{
        snake2.playing=0;
    }

}
void removeNeutral(Cell newHead){
    char found=0;
    for(int i=0;i<neutralCellsSize&&!found;i++){
        if(newHead.x==neutralCells[i].x && newHead.y==neutralCells[i].y){
            neutralCells[i]=neutralCells[--neutralCellsSize];
            found=1;
        }
    }
}
int moveSnake(Snake *snake) {
    unsigned int x, y;
    switch (snake->orientation) {
        case DOWN:
            x = 0;
            y = 1;
            break;
        case UP:
            y = -1;
            x = 0;
            break;
        case LEFT:
            x = -1;
            y = 0;
            break;
        case RIGHT:
            y = 0;
            x = 1;
            break;
    }
    Cell newHead;
    newHead.x = (snake->cells).array[(snake->cells).head].x + x;
    newHead.y = (snake->cells).array[(snake->cells).head].y + y;
    newHead.cellType = map[newHead.y][newHead.x];
    Cell oldTail;
    oldTail.x = (snake->cells).array[(snake->cells).tail].x;
    oldTail.y = (snake->cells).array[(snake->cells).tail].y;
    oldTail.cellType = NEUTRAL;
    if (newHead.x >= X_SIZE || newHead.x < 0 || newHead.y >= Y_SIZE || newHead.y < 0 ||
        map[newHead.y][newHead.x] == DEATH) {
        sys_beep(175,333);
        sys_beep(165,333);
        sys_beep(155,1000);
        return -1;
    }
    int flag=0;
    if (newHead.cellType == NEUTRAL) {
        map[oldTail.y][oldTail.x] = NEUTRAL;
        cpyCell(&(neutralCells[neutralCellsSize++]), oldTail);
        drawCellBackground(oldTail.x, oldTail.y);
        updateTail(&(snake->cells));
        flag--;
    } else{
        sys_beep(784,100);
        sys_beep(1047,150);
    }
    newHead.cellType = DEATH;
    updateHead(&(snake->cells), newHead);
    paintCircle(newHead.x, newHead.y + 2, snake->headColor);
    map[newHead.y][newHead.x] = DEATH;
    removeNeutral(newHead);
    if(!isEmpty(snake->cells)){
        paintCircle(newHead.x - x, newHead.y - y + 2, snake->bodyColor);
    }
    return ++flag;
}

void finishGame(){
    print("\nGAME OVER! Press ESC to exit", 0xffffff, 2);
    char input[2] = {0};
    while (input[0] != 1) {
        sys_hardRead(1,input,1);
    }
}

int play(Snake* player, const char input, const char up, const char down, const char left, const char right){
    if(input == left){
        if(player->orientation!=RIGHT){
            player->orientation = LEFT;
        }
    }
    else if(input == right){
        if(player->orientation!=LEFT){
            player->orientation = RIGHT;
        }
    }
    else if(input == up){
        if(player->orientation!=DOWN){
            player->orientation = UP;
        }
    }
    else if(input == down){
        if(player->orientation!=UP){
            player->orientation = DOWN;
        }
    }
    char flag;
    flag = moveSnake(player);
    return flag;
}

int snake(SNAKE_MODE mode){
    unsigned int player1Score = 0, player2Score = 0;
    initializeMap(mode);

    Coords gridSize = {X_SIZE, Y_SIZE};
    drawEntireBackground(gridSize);

    char appleSpawned = 0;
    Cell apple = {0, 0, APPLE};
    char input=0;
    int flag;
    while (input!=1){

        sys_waitNMillis(150);

        sys_hardRead(1,&input,1);
        flag = play(&snake1, input, 'w', 's', 'a', 'd');
        if(flag == 1){
            appleSpawned=0;
            player1Score++;
        } else if(flag == -1){
            finishGame();
            break;
        }

        sys_hardRead(1,&input,1);
        if(snake2.playing==1){
            flag = play(&snake2, input, 'i', 'k', 'j', 'l');
            if(flag == 1){
                appleSpawned=0;
                player2Score++;
            } else if(flag == -1){
                finishGame();
                break;
            }
        }

        //If there's no apple, spawn one and paint it
        if(appleSpawned==0 && neutralCellsSize > 0){
            spawnApple(&apple);
            appleSpawned = 1;
            paintCircle(apple.x, apple.y + 2, APPLECOLOR);
        } else if(appleSpawned == 0){
            finishGame();
            break;
        }

        sys_blankArea(0,748,0,36,0);
        sys_resetCursor();

        print("Score: ", snake1.bodyColor, 2);
        char str[32];
        uintToBase(player1Score, str, 10);
        print(str, snake1.bodyColor, 2);

        if(mode==DUOS){
            print("\nScore: ", snake2.bodyColor, 2);
            char str[32];
            uintToBase(player2Score, str, 10);
            print(str, snake2.bodyColor, 2);
        }
    }
    return 0;
}