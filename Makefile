CC = clang
CFLAGS = -std=c99 -Wall -Werror -fsanitize=address -lSDL2  -I./vendor/include -ldl -lm -lGL -g -O0 
TARGET = build/hayal
SRC = src/main_linux.c vendor/glad.c vendor/stb_image.c 

compile-dev:
	$(CC) ${SRC} $(CFLAGS) -o ${TARGET}

debug: compile-dev
	lldb ./${TARGET}

