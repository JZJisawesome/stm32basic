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
release: CFLAGS += -O3  $(COMMONOPTIMIZINGCFLAGS) -DNDEBUG
debug: CFLAGS += -DDEBUG -g -Og
release: $(BUILDDIR)/video/video.hex $(BUILDDIR)/cpu/cpu.hex
debug: $(BUILDDIR)/video/video.hex $(BUILDDIR)/cpu/cpu.hex

#Linking

$(BUILDDIR)/cpu/cpu.hex: $(BUILDDIR)/cpu/cpu | $(BUILDDIR)/cpu
	arm-none-eabi-objcopy -O ihex $(BUILDDIR)/cpu/cpu $(BUILDDIR)/cpu/cpu.hex

$(BUILDDIR)/video/video.hex: $(BUILDDIR)/video/video | $(BUILDDIR)/video
	arm-none-eabi-objcopy -O ihex $(BUILDDIR)/video/video $(BUILDDIR)/video/video.hex

$(BUILDDIR)/cpu/cpu: $(BUILDDIR)/common/bluepill.o | $(BUILDDIR)/cpu
	arm-none-eabi-ld --print-memory-usage -T bluepill.ld $(BUILDDIR)/common/bluepill.o -o $(BUILDDIR)/cpu/cpu

$(BUILDDIR)/video/video: $(BUILDDIR)/common/bluepill.o | $(BUILDDIR)/video
	arm-none-eabi-ld --print-memory-usage -T bluepill.ld $(BUILDDIR)/common/bluepill.o -o $(BUILDDIR)/video/video

#Object files

#Common

$(BUILDDIR)/common/bluepill.o: $(SRCDIR)/common/bluepill.S $(DEPS) | $(BUILDDIR)/common
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/common/bluepill.S -o $(BUILDDIR)/common/bluepill.o

#CPU



#Video



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
flash_cpu:
	st-flash --format=ihex write $(BUILDDIR)/cpu/cpu.hex
	
.PHONY: flash_video
flash_video:
	st-flash --format=ihex write $(BUILDDIR)/video/video.hex
