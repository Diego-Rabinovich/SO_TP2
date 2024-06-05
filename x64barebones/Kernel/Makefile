include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
MMFL=memoryManager/freeList.c
MMBD=memoryManager/buddy.c
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTSMMFL=$(MMFL:.c=.o)
OBJECTSMMBD=$(MMBD:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTSMMFL)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTSMMFL) $(STATICLIBS)
	#$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTSMMFL) $(OBJECTS_ASM) $(STATICLIBS)

buddy: $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTSMMBD)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTSMMBD)
	#$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTSMMBD) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin *.elf memoryManager/*.o
.PHONY: all clean buddy
