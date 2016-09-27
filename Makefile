CC=i686-elf-gcc
CXX=i686-elf-g++
CFLAGS=-std=c99 -ffreestanding -O2 -Wall -Wextra
BUILD_DIR=build

all: $BUILD_DIR/kernel.o

$BUILD_DIR:
	mkdir -p build

$BUILD_DIR/kernel.o: kernel.c $(BUILD_DIR)
	$(CC) -c kernel.c -o $(BUILD_DIR)/kernel.o $(CFLAGS)
