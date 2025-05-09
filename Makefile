CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

# Include directories — project headers first
INCLUDES=-Iinclude -ICMSIS -IFreeRTOS/include -IFreeRTOS/portable/GCC/ARM_CM3

CFLAGS=-mcpu=cortex-m3 -mthumb -Wall -Os -ffreestanding -nostdlib $(INCLUDES)
LDFLAGS=-Tld/stm32f103.ld -nostdlib -Wl,--gc-sections

CFLAGS += -Wno-implicit-function-declaration
LDFLAGS += -lc -lgcc

# Source files
SRC = $(wildcard src/*.c CMSIS/*.c FreeRTOS/*.c FreeRTOS/portable/GCC/ARM_CM3/*.c)
OBJ = $(SRC:.c=.o)

all: belfhym.elf belfhym.bin

belfhym.elf: startup/startup_stm32f103.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

belfhym.bin: belfhym.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJ) startup/*.o belfhym.elf belfhym.bin
