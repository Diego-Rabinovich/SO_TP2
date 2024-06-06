include ../Makefile.inc

MODULE=0000-userCode.bin
#MODULE_ELF=0000-userCode.elf
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/[^_]*.asm)
SOURCES_SHELL=$(wildcard ../Shell/[^_]*.c)
SOURCES_GRAPHICS=$(wildcard ../Graphics/[^_]*.c)
SOURCES_SNAKE=$(wildcard ../Snake/[^_]*.c)
SOURCES_TEST=$(wildcard ../Testing/*.c)
SOURCES_MATH=$(wildcard ../Math/[^_]*.c)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM) $(SOURCES_SHELL) $(SOURCES_GRAPHICS) $(SOURCES_SNAKE) $(SOURCES_MATH) $(SOURCES_TEST)
	$(GCC) $(GCCFLAGS) -T user.ld _loader.c $(SOURCES) $(SOURCES_SHELL) $(SOURCES_GRAPHICS) $(SOURCES_SNAKE) $(SOURCES_MATH) $(SOURCES_TEST) $(OBJECTS_ASM) -o ../$(MODULE)
	#$(GCC) $(GCCFLAGS) -T user.ld _loader.c -Wl,--oformat=elf64-x86-64 $(SOURCES) $(SOURCES_SHELL) $(SOURCES_GRAPHICS) $(SOURCES_SNAKE) $(SOURCES_TEST) $(SOURCES_MATH) $(OBJECTS_ASM) -o ../$(MODULE_ELF)

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o *.bin *.elf

.PHONY: all clean print
