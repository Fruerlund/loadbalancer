CC = gcc # Flag for implicit rules

CFLAGS = -g -c -I src/headers # Flag for implicit rules. Turn on debug info
LDFLAGS=  -Tlinker/Link.ld -m elf_i386

C_SOURCES = $(wildcard src/*.c)
LIB_SOURCES= $(wildcard lib/*.c)

OBJ = ${C_SOURCES:.c=.o)}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

all: ${OBJ}
	ld ${LDFLAGS} -o main.elf $^

lib: ${LIB_SOURCES}
	$CC} ${CFLAGS} -c $< -o /build $@

clean:
	echo "Clean"
	rm build/*
