# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 Masoud Bolhassani

# Project name
PROJECT := belfhym

# Directories
SRC_DIR       := src
BUILD_DIR     := build
BIN_DIR       := bin
INCLUDE_DIR   := include
FREERTOS_DIR  := FreeRTOS
CMSIS_DIR     := CMSIS
LD_SCRIPT     := ld/stm32f103.ld

# Toolchain
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

# Flags
CFLAGS := -Wall -Wextra -O2 -mcpu=cortex-m3 -mthumb -nostdlib -ffreestanding
CFLAGS += -DSTM32F103xB -I$(INCLUDE_DIR) -I$(CMSIS_DIR)
CFLAGS += -I$(FREERTOS_DIR)/include -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3
LDFLAGS := -T$(LD_SCRIPT)

# Sources
SRC_SUBDIRS := actuators app communication control core drivers interface logging logic monitoring power safety sensors
SRC_DIRS := $(addprefix $(SRC_DIR)/,$(SRC_SUBDIRS))
USER_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

FREERTOS_SRCS := \
    $(FREERTOS_DIR)/event_groups.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/stream_buffer.c \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
    $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c

CMSIS_SRCS := \
    $(CMSIS_DIR)/startup_stm32f103xb.s \
    $(CMSIS_DIR)/system_stm32f1xx.c

SRCS := $(USER_SRCS) $(FREERTOS_SRCS) $(CMSIS_SRCS)

# Object files
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS)))
OBJS += $(patsubst %.s,$(BUILD_DIR)/%.o,$(filter %.s,$(SRCS)))

# Output files
TARGET    := $(BIN_DIR)/$(PROJECT)
BIN_FILE  := $(TARGET).bin

# Default target
.PHONY: all
all: $(TARGET)

# Linking
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Compile .c files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile .s files
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) -mcpu=cortex-m3 -mthumb -c $< -o $@

# Create bin directory
$(BIN_DIR):
	@mkdir -p $@

# Convert ELF to .bin
$(BIN_FILE): $(TARGET)
	$(OBJCOPY) -O binary $< $@

# Deployment
METHOD ?= stlink

.PHONY: deploy
deploy: $(BIN_FILE)
ifeq ($(METHOD),stlink)
	st-flash write $(BIN_FILE) 0x8000000
else ifeq ($(METHOD),openocd)
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(BIN_FILE) 0x8000000 verify reset exit"
else ifeq ($(METHOD),gdb)
	@echo "Starting OpenOCD server in background..."
	@pkill openocd || true
	@openocd -f interface/stlink.cfg -f target/stm32f1x.cfg &
	@sleep 1
	arm-none-eabi-gdb $(TARGET) -ex "target extended-remote localhost:3333" \
	                  -ex "monitor reset halt" \
	                  -ex "load" \
	                  -ex "monitor reset run" \
	                  -ex "quit"
else
	$(error Unknown METHOD: $(METHOD). Use METHOD=stlink|openocd|gdb)
endif

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
