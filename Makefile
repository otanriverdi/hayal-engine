CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Werror -fsanitize=address -lSDL2  -I./vendor/include -I./src/include -ldl -lm -lGL -g -O0 -lfreetype -I/usr/include/freetype2 
TARGET = build/hayal
SRC = src/main_linux.cpp vendor/glad.cpp vendor/stb.cpp vendor/miniaudio.cpp

compile:
	$(CXX) ${SRC} $(CXXFLAGS) -o ${TARGET}

debug: compile
	lldb ./${TARGET}
