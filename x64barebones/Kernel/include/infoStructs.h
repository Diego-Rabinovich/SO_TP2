#define READ 0
#define WRITE 1
#include "lib.h"

typedef enum PState {RUNNING, BLOCKED, READY, TERMINATED} PState;

typedef int (*Main)(int argc, char **args);

typedef struct ProcessInfo {
    uint16_t pid;
    uint16_t parent_pid;
    void *rsb;
    void *rsp;
    char *name;
    uint8_t piority;
    PState p_state;
    uint8_t fg;
} ProcessInfo;

typedef struct ProcessInfoArray {
	uint16_t length;
	ProcessInfo *array;
} ProcessInfoArray;


#define STDIN 0
#define STDOUT 1
#define STDERR 2
