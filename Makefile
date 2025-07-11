CC = clang
CFLAGS = -std=c99 -Wall -Werror -fsanitize=address -lSDL2  -I./vendor/include -I./src/include -ldl -lm -lGL -g -O0 -lfreetype -I/usr/include/freetype2 
TARGET = build/hayal
SRC = src/main_linux.c vendor/glad.c vendor/stb.c vendor/miniaudio.c

compile:
	$(CC) ${SRC} $(CFLAGS) -o ${TARGET}

debug: compile
	lldb ./${TARGET}
