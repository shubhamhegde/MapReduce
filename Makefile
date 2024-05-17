CC = gcc
INCLUDE_DIR = ./include/
SRC_DIR = ./src/
BUILD_DIR = ./build/


all: 
	${CC} -I ${INCLUDE_DIR} ${SRC_DIR}/main.c -o ${BUILD_DIR}/main

clean:
	rm ./build/main
