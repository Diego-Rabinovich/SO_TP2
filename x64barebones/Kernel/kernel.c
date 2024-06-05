#include "include/lib.h"
#include "include/moduleLoader.h"
#include "include/idtLoader.h"
#include "include/videoDriver.h"
#include "include/interrupts.h"
#include "include/audioDriver.h"
#include "include/memoryManager.h"

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
    memInit((void *) 0x600000, 128*1024*1024); //Le damos 128MB

    return getStackBase();
}

void startUpMusic(){
    beep(1318,250);
    beep(1245,250);
    beep(1318,250);
    beep(1245,250);
    beep(1318,250);
    beep(987,250);
    beep(1175,250);
    beep(1046,250);
    beep(880,500);
}

int main(){
    load_idt();
    startUpMusic();
    resetScreen();
    loadUserContext();
    return 0;
}
