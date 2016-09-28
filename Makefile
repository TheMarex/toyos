CC=i686-elf-gcc
QEMU=qemu-system-i386
AS=i686-elf-as
CXX=i686-elf-g++
CFLAGS=-std=c99 -ffreestanding -O2 -Wall -Wextra -nostdlib
TEST_CFLAGS=$(CFLAGS) -DENABLE_TESTS
BUILD_DIR=build

# Standart traget: build multiboot enabled kernel binary
#

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/bootstrap.o: bootstrap.s $(BUILD_DIR)
	$(AS) bootstrap.s -o $(BUILD_DIR)/bootstrap.o

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.o $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o kernel.ld
	$(CC) -T kernel.ld -o $(BUILD_DIR)/kernel.bin $(CFLAGS) $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/kernel.o -lgcc

# Tests
#

$(BUILD_DIR)/tests:
	mkdir -p $(BUILD_DIR)/tests

$(BUILD_DIR)/tests/%.o: tests/%.c $(BUILD_DIR)/tests
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

$(BUILD_DIR)/tests/kernel.o: kernel.c $(BUILD_DIR)/tests
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

# every test gets an own kernel image to ensure consistent state
$(BUILD_DIR)/tests/kernel_%.bin: $(BUILD_DIR)/tests/%.o $(BUILD_DIR)/tests/kernel.o $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o kernel.ld
	$(CC) -T kernel.ld -o $@ $(TEST_CFLAGS) $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/tests/kernel.o $< -lgcc

# Convinience
#

qemu: $(BUILD_DIR)/kernel.bin
	$(QEMU) -kernel $(BUILD_DIR)/kernel.bin

tests: $(BUILD_DIR)/tests/kernel_vga_test.bin
	$(QEMU) -kernel $(BUILD_DIR)/tests/kernel_vga_test.bin

clean:
	rm $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/*.bin
	rm $(BUILD_DIR)/tests/*.bin
	rm $(BUILD_DIR)/tests/*.o

.PHONY: qemu tests clean
