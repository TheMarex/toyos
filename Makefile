CC=i686-elf-gcc
QEMU=qemu-system-i386
AS=i686-elf-as
CXX=i686-elf-g++
CFLAGS=-std=c99 -ffreestanding -Og -ggdb3 -Wall -Wextra -nostdlib
SFLAGS=-g
TEST_CFLAGS=$(CFLAGS) -DENABLE_TESTS
ARCH=i386
BUILD_DIR=build/$(ARCH)
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

# Standart traget: build multiboot enabled kernel binary
#

all: i386

i386: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/%.o: kernel/%.c $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/%.o: arch/i386/%.s $(BUILD_DIR)
	$(AS) $< $(SFLAGS) -o $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.o $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/gdt.o arch/i386/kernel.ld $(BUILD_DIR)/crti.o $(BUILD_DIR)/crtn.o
	$(CC) -T arch/i386/kernel.ld -o $(BUILD_DIR)/kernel.bin $(CFLAGS) $(BUILD_DIR)/crti.o $(CRTBEGIN) $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/gdt.o $(BUILD_DIR)/kernel.o $(CRTEND) $(BUILD_DIR)/crtn.o -lgcc

# Tests
#

$(BUILD_DIR)/tests:
	mkdir -p $(BUILD_DIR)/tests

$(BUILD_DIR)/tests/%.o: tests/%.c $(BUILD_DIR)/tests
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

$(BUILD_DIR)/tests/kernel.o: kernel/kernel.c $(BUILD_DIR)/tests
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

# every test gets an own kernel image to ensure consistent state
$(BUILD_DIR)/tests/kernel_%.bin: $(BUILD_DIR)/tests/%.o $(BUILD_DIR)/tests/kernel.o $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/gdt.o arch/i386/kernel.ld $(BUILD_DIR)/crti.o $(BUILD_DIR)/crtn.o
	$(CC) -T arch/i386/kernel.ld -o $@ $(TEST_CFLAGS) $(BUILD_DIR)/crti.o $(CRTBEGIN) $(BUILD_DIR)/bootstrap.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/gdt.o $(BUILD_DIR)/tests/kernel.o $< $(CRTEND) $(BUILD_DIR)/crtn.o -lgcc

# Convinience
#

qemu: $(BUILD_DIR)/kernel.bin
	$(QEMU) -kernel $(BUILD_DIR)/kernel.bin

test_vga: $(BUILD_DIR)/tests/kernel_vga_test.bin
	$(QEMU) -s -S -kernel $(BUILD_DIR)/tests/kernel_vga_test.bin

test_gdt: $(BUILD_DIR)/tests/kernel_gdt_test.bin
	$(QEMU) -s -S -kernel $(BUILD_DIR)/tests/kernel_gdt_test.bin

tests: test_vga test_gdt

clean:
	rm $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/*.bin
	rm $(BUILD_DIR)/tests/*.bin
	rm $(BUILD_DIR)/tests/*.o

.PHONY: qemu tests test_vga test_gdt clean i386
