#ifndef SO_TP2_PROCESSLIST_H
#define SO_TP2_PROCESSLIST_H

#include "include/lib.h"

typedef enum PState {RUNNING, BLOCKED, READY, TERMINATED} PState;

typedef struct Registers{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
} Registers;

typedef struct PCB{
    uint32_t id;
    PState p_state;

    uint32_t priority;
    uint32_t execution_count;

    Registers registers;

    uint64_t base_pointer;
    uint32_t mem_size;
} PCB;

typedef struct QueueNode{
    PCB pcb;
    QueueNode* next;
    QueueNode* prev;
} QueueNode;

typedef struct QueueHeader {
    QueueNode* first;
    QueueNode* last;
    uint32_t count;
    QueueNode* next;
}QueueHeader;

PCB* PopProcess(QueueHeader* queue);
void PushProcess(QueueHeader* queue, PCB* process_pcb);

PCB* Iterate(QueueHeader* queue);
uint32_t RestartIterate(QueueHeader* queue);

void RemoveProcess(QueueHeader * queue, uint32_t pid);
void TerminateProcess(QueueHeader * queue, uint32_t pid);

#endif //SO_TP2_PROCESSLIST_H
