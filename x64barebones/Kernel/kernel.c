// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/lib.h"
#include "include/moduleLoader.h"
#include "include/idtLoader.h"
#include "include/videoDriver.h"
#include "include/interrupts.h"
#include "include/audioDriver.h"
#include "include/keyboardDriver.h"
#include "include/memoryManager.h"
#include "include/scheduler.h"
#include "fileDescriptor.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;


static void * const userCodeAddress = (void*)0x400000;
static void * const userDataAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
    memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
    return (void*)(
            (uint64_t)&endOfKernel
                       + PageSize * 8				//The size of the stack itself, 32KiB
                       - sizeof(uint64_t)			//Begin at the top of the stack
    );
}

void * initializeKernelBinary()
{
    void * moduleAddresses[] = {
            userCodeAddress,
            userDataAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);
    memInit((void *) 0x600000, 256*1024*1024); //Le damos 256MB

    return getStackBase();
}

void startUpMusic(){
    beep(450,500);
}

int trivial(int argc, char **args){
    char * argv[] = {NULL};
    int16_t* fds = memAlloc(3*sizeof(int16_t));
    getFDs(fds);
    createProcess(userCodeAddress, argv, "init", 0, fds);
    memFree(fds);
    while (1) {
        _hlt();
    }
}



int main(){
    resetScreen();
    schedulerInit();
    initiateFDs();
    char * args[] = {NULL};
    int16_t fds[3] = {STDIN, STDOUT, STDERR};
    createProcess(trivial, args, "trivial", 0, fds);     //Halt process in case of no active processes
    load_idt();
    callTimerTick();
    return 0;
}
