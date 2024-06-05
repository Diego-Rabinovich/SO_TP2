include ../Makefile.inc

MODULE=0000-userCode.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/[^_]*.asm)
SOURCES_SHELL=$(wildcard ../Shell/[^_]*.c)
SOURCES_GRAPHICS=$(wildcard ../Graphics/[^_]*.c)
SOURCES_SNAKE=$(wildcard ../Snake/[^_]*.c)
SOURCES_MATH=$(wildcard ../Math/[^_]*.c)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM) $(SOURCES_SHELL) $(SOURCES_GRAPHICS) $(SOURCES_SNAKE) $(SOURCES_MATH)
	$(GCC) $(GCCFLAGS) -T user.ld _loader.c $(SOURCES) $(SOURCES_SHELL) $(SOURCES_GRAPHICS) $(SOURCES_SNAKE) $(SOURCES_MATH) $(OBJECTS_ASM) -o ../$(MODULE)

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o *.bin

.PHONY: all clean print