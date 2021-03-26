#Lots borrowed from various other projects 

BUILDDIR = ./build/
SRCDIR = ./src/
INCDIR =./include/

CC = arm-none-eabi-gcc
CFLAGS = -Wall -DF_CPU=16000000 -mthumb -mcpu=cortex-m3 -std=gnu17 -ffreestanding
COMMONOPTIMIZINGCFLAGS = -flto -fuse-linker-plugin -fmerge-all-constants
INC = -I$(INCDIR)

DEPS = $(INCDIR)/common/bluepill.h
.PHONY: release debug
release: CFLAGS += -O3 -DNDEBUG $(COMMONOPTIMIZINGCFLAGS)
debug: CFLAGS += -DDEBUG -g -Og
release: $(BUILDDIR)/video/video.hex $(BUILDDIR)/cpu/cpu.hex
debug: $(BUILDDIR)/video/video.hex $(BUILDDIR)/cpu/cpu.hex

#Linking

$(BUILDDIR)/cpu/cpu.hex: $(BUILDDIR)/cpu/cpu | $(BUILDDIR)/cpu
	arm-none-eabi-objcopy -O ihex $(BUILDDIR)/cpu/cpu $(BUILDDIR)/cpu/cpu.hex

$(BUILDDIR)/video/video.hex: $(BUILDDIR)/video/video | $(BUILDDIR)/video
	arm-none-eabi-objcopy -O ihex $(BUILDDIR)/video/video $(BUILDDIR)/video/video.hex

$(BUILDDIR)/cpu/cpu: $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/cpu/cpu.o | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) -ffreestanding -nostdlib -Wl,--print-memory-usage -Wl,-T,bluepill.ld $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/cpu/cpu.o -o $(BUILDDIR)/cpu/cpu

$(BUILDDIR)/video/video: $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/video/video.o | $(BUILDDIR)/video
	$(CC) $(CFLAGS) -ffreestanding -nostdlib -Wl,--print-memory-usage -Wl,-T,bluepill.ld $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/video/video.o -o $(BUILDDIR)/video/video

#Object files

#Common

$(BUILDDIR)/common/bluepill.o: $(SRCDIR)/common/bluepill.S $(DEPS) | $(BUILDDIR)/common
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/common/bluepill.S -o $(BUILDDIR)/common/bluepill.o

#CPU

$(BUILDDIR)/cpu/cpu.o: $(SRCDIR)/cpu/cpu.c $(DEPS) | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/cpu/cpu.c -o $(BUILDDIR)/cpu/cpu.o

#Video

$(BUILDDIR)/video/video.o: $(SRCDIR)/video/video.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/video.c -o $(BUILDDIR)/video/video.o

#Build directories

$(BUILDDIR)/common: $(BUILDDIR)
	mkdir -p $(BUILDDIR)/common

$(BUILDDIR)/cpu: $(BUILDDIR)
	mkdir -p $(BUILDDIR)/cpu

$(BUILDDIR)/video: $(BUILDDIR)
	mkdir -p $(BUILDDIR)/video

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

#Special phonies

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: flash_cpu
flash_cpu: $(BUILDDIR)/cpu/cpu.hex
	st-flash --format=ihex write $(BUILDDIR)/cpu/cpu.hex
	
.PHONY: flash_video
flash_video: $(BUILDDIR)/video/video.hex
	st-flash --format=ihex write $(BUILDDIR)/video/video.hex
