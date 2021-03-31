#Lots borrowed from various other projects
#TODO make automatic so that file names dont need to be constantly added

BUILDDIR = ./build/
SRCDIR = ./src/
INCDIR =./include/

CC = arm-none-eabi-gcc
CFLAGS = -Wall -DF_CPU=16000000 -mthumb -mcpu=cortex-m3 -std=gnu17 -ffreestanding
COMMONOPTIMIZINGCFLAGS = -flto -fuse-linker-plugin -fmerge-all-constants
INC = -I$(INCDIR) -I$(INCDIR)/common -I$(INCDIR)/cpu -I$(INCDIR)/video
LINKEROPTS = -ffreestanding -nostdlib -Wl,--print-memory-usage -Wl,-T,bluepill.ld

#TODO seperate out into common deps, cpu deps, and video deps
DEPS = $(INCDIR)/common/bluepill.h $(INCDIR)/cpu/basi.h $(INCDIR)/cpu/ps2.h $(INCDIR)/cpu/spiio_cpu.h $(INCDIR)/cpu/vhal.h $(INCDIR)/video/composite.h $(INCDIR)/video/processing.h $(INCDIR)/video/ps2uart.h $(INCDIR)/video/softrenderer.h $(INCDIR)/video/spiio_video.h $(INCDIR)/video/vincent.h

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

$(BUILDDIR)/cpu/cpu: $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/cpu/cpu.o $(BUILDDIR)/cpu/basi.o $(BUILDDIR)/cpu/ps2.o $(BUILDDIR)/cpu/spiio_cpu.o $(BUILDDIR)/cpu/vhal.o | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(LINKEROPTS) $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/cpu/cpu.o $(BUILDDIR)/cpu/basi.o $(BUILDDIR)/cpu/ps2.o $(BUILDDIR)/cpu/spiio_cpu.o $(BUILDDIR)/cpu/vhal.o -o $(BUILDDIR)/cpu/cpu

$(BUILDDIR)/video/video: $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/video/video.o $(BUILDDIR)/video/composite.o $(BUILDDIR)/video/processing.o $(BUILDDIR)/video/ps2uart.o $(BUILDDIR)/video/softrenderer.o $(BUILDDIR)/video/spiio_video.o | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(LINKEROPTS) $(BUILDDIR)/common/bluepill.o $(BUILDDIR)/video/video.o $(BUILDDIR)/video/composite.o $(BUILDDIR)/video/processing.o $(BUILDDIR)/video/ps2uart.o $(BUILDDIR)/video/softrenderer.o $(BUILDDIR)/video/spiio_video.o -o $(BUILDDIR)/video/video

#Object files

#Common

$(BUILDDIR)/common/bluepill.o: $(SRCDIR)/common/bluepill.S $(DEPS) | $(BUILDDIR)/common
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/common/bluepill.S -o $(BUILDDIR)/common/bluepill.o

#CPU

$(BUILDDIR)/cpu/cpu.o: $(SRCDIR)/cpu/cpu.c $(DEPS) | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/cpu/cpu.c -o $(BUILDDIR)/cpu/cpu.o

$(BUILDDIR)/cpu/basi.o: $(SRCDIR)/cpu/basi.c $(DEPS) | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/cpu/basi.c -o $(BUILDDIR)/cpu/basi.o

$(BUILDDIR)/cpu/ps2.o: $(SRCDIR)/cpu/ps2.c $(DEPS) | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/cpu/ps2.c -o $(BUILDDIR)/cpu/ps2.o

$(BUILDDIR)/cpu/spiio_cpu.o: $(SRCDIR)/cpu/spiio_cpu.c $(DEPS) | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/cpu/spiio_cpu.c -o $(BUILDDIR)/cpu/spiio_cpu.o

$(BUILDDIR)/cpu/vhal.o: $(SRCDIR)/cpu/vhal.c $(DEPS) | $(BUILDDIR)/cpu
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/cpu/vhal.c -o $(BUILDDIR)/cpu/vhal.o

#Video

$(BUILDDIR)/video/video.o: $(SRCDIR)/video/video.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/video.c -o $(BUILDDIR)/video/video.o

$(BUILDDIR)/video/composite.o: $(SRCDIR)/video/composite.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/composite.c -o $(BUILDDIR)/video/composite.o

$(BUILDDIR)/video/processing.o: $(SRCDIR)/video/processing.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/processing.c -o $(BUILDDIR)/video/processing.o

$(BUILDDIR)/video/ps2uart.o: $(SRCDIR)/video/ps2uart.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/ps2uart.c -o $(BUILDDIR)/video/ps2uart.o

$(BUILDDIR)/video/softrenderer.o: $(SRCDIR)/video/softrenderer.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/softrenderer.c -o $(BUILDDIR)/video/softrenderer.o

$(BUILDDIR)/video/spiio_video.o: $(SRCDIR)/video/spiio_video.c $(DEPS) | $(BUILDDIR)/video
	$(CC) $(CFLAGS) $(INC) -c $(SRCDIR)/video/spiio_video.c -o $(BUILDDIR)/video/spiio_video.o

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
	st-flash reset

.PHONY: flash_video
flash_video: $(BUILDDIR)/video/video.hex
	st-flash --format=ihex write $(BUILDDIR)/video/video.hex
	st-flash reset
