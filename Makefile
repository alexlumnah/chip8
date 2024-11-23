# Define the compiled
CC = gcc

# Compiler Flags:
CFLAGS = -g -Wall -Wpedantic -Wextra -fsanitize=address,undefined,signed-integer-overflow
RAYFLAGS = lib/libraylib.a -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

TEST_SRC = $(wildcard test/*.c)
TEST_OBJ = $(TEST_SRC:.c=.o)

all: main

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o chip8 $^ $(CFLAGS) $(LDFLAGS) $(RAYFLAGS)

clean:
	rm chip8 $(OBJ)

tidy:
	clang-tidy src/* --

cppcheck:
	cppcheck --enable=portability --check-level=exhaustive --enable=style src/*.c
