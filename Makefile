# MyOS Makefile
# Gereksinimler: g++ (cross-compiler önerilir), grub-mkrescue, xorriso, qemu-system-i386

CXX      = g++
CXXFLAGS = -m32 -ffreestanding -fno-exceptions -fno-rtti -nostdlib \
           -fno-stack-protector -Wall -Wextra -std=c++17

SRCS = kernel/kernel.cpp kernel/process.cpp kernel/memory.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean run iso

all: myos.bin

# Nesne dosyalarını derle
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Kernel binary'sini linkle
myos.bin: $(OBJS)
	$(CXX) -m32 -nostdlib -T linker.ld -o $@ $(OBJS)

# ISO oluştur (GRUB ile)
iso: myos.bin
	mkdir -p iso/boot/grub
	cp myos.bin iso/boot/myos.bin
	cp boot/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

# QEMU ile çalıştır
run: iso
	qemu-system-i386 -cdrom myos.iso -nographic

clean:
	rm -f $(OBJS) myos.bin myos.iso
	rm -rf iso
