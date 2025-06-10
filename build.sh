#!/usr/bin/env bash

clang src/main_linux.c vendor/glad.c -std=c99 -Wall -Werror -fsanitize=address -lSDL2  -I./vendor/include -ldl -lm -lGL -o build/alchemo 
