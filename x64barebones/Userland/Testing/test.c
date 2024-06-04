#include "stdio.h"
#include "stdlib.h"
#include "memoryManager.h"
#include "assert.h"

#define ALLMEM 4*1024*1024
void firstLevel(void);
void secondLevel(void);
void finalBoss(void);

int main(void){
    finalBoss();
}

void firstLevel(void){
    void * allMemory = malloc(ALLMEM);
    memInit(allMemory, ALLMEM);
    char * arrays[26] ;

    for (int j = 0 ; j < 26; j++){
        arrays[j] = memAlloc(4100 * sizeof(char));
        int i;
        for (i = 0 ;  i < 4100 ; i++){
            sprintf( &arrays[j][i],"%c", 'a'+j);
        }
        arrays[j][i] = 0;
    }
    for (int i = 0; i < 26 ; i++){
        printf("%s", arrays[i]);
    }

    free(allMemory);
}


void secondLevel(void){
    void * allMemory = malloc(ALLMEM);
    memInit(allMemory, ALLMEM);
    char * arrays[26] ;

    for (int j = 0 ; j < 26; j++){
        arrays[j] = memAlloc(4100 * sizeof(char));
        int i;
        for (i = 0 ;  i < 4100 ; i++){
            sprintf( &arrays[j][i],"%c", 'a'+j);
        }
        arrays[j][i] = 0;
    }


    for (int k = 0; k < 26 ; k++){
        if(k == 25){
            memFree(arrays[k]);
        }else {
            memFree(arrays[k]);
        }
    }

    free(allMemory);
}

void finalBoss(void){
    void * allMemory = malloc(ALLMEM);
    memInit(allMemory, ALLMEM);
    char * arrays[26] ;
    char * arrays2[26] ;

    for (int j = 0 ; j < 26; j++){
        arrays[j] = memAlloc(4100 * sizeof(char));
        int i;
        for (i = 0 ;  i < 4100 ; i++){
            sprintf( &arrays[j][i],"%c", 'a'+j);
        }
        arrays[j][i] = 0;
    }

    for (int k = 0; k < 26 ; k+=2){
        memFree(arrays[k]);
    }

    for (int j = 0 ; j < 26; j++){
        arrays2[j] = memAlloc(4100 * sizeof(char));
        int i;
        for (i = 0 ;  i < 4100 ; i++){
            sprintf( &arrays2[j][i],"%c", 'a'+j);
        }
        arrays2[j][i] = 0;
    }

    for (int k = 1; k < 26 ; k+=2){
        memFree(arrays2[k]);
    }

    for (int k = 1; k < 26 ; k+=2){
        if(k == 25){
            memFree(arrays[k]);
        }else {
            memFree(arrays[k]);
        }
    }

    for (int k = 0; k < 26 ; k+=2){
        if(k == 24){
            memFree(arrays2[k]);
        } else {
            memFree(arrays2[k]);
        }
    }

    free(allMemory);
}