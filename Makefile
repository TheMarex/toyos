CC=i686-elf-gcc
AS=i686-elf-as
CXX=i686-elf-g++
CFLAGS=-std=c99 -ffreestanding -O2 -Wall -Wextra -nostdlib
BUILD_DIR=build

all: $(BUILD_DIR)/kernel.bin

$BUILD_DIR:
	mkdir -p build

$(BUILD_DIR)/kernel.o: kernel.c $(BUILD_DIR)
	$(CC) -c kernel.c -o $(BUILD_DIR)/kernel.o $(CFLAGS)

$(BUILD_DIR)/bootstrap.o: bootstrap.s $(BUILD_DIR)
	$(AS) bootstrap.s -o $(BUILD_DIR)/bootstrap.o

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.o $(BUILD_DIR)/bootstrap.o kernel.ld
	$(CC) -T kernel.ld -o $(BUILD_DIR)/kernel.bin $(CFLAGS) $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/kernel.o -lgcc
