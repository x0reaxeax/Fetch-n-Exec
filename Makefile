CC		= gcc
SRC		= main.c
OBJ		= syscalls.o
OUT		= binary.elf
CFLAGS	= -nostdlib
ASMCC	= nasm
ASMFMT	= -f elf64
ASMSRC	= syscall_helper.asm

all: syscall_helper binary

syscall_helper:
	$(ASMCC) $(ASMFMT) -o $(OBJ) $(ASMSRC)

binary:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(OBJ)

clean:
	rm $(OBJ) $(OUT)