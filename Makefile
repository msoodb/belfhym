# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 Masoud Bolhassani

PROJECT := belfhym

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include
FREERTOS_DIR := FreeRTOS
CMSIS_DIR := CMSIS

# Toolchain
CC := arm-none-eabi-gcc

# Flags
CFLAGS_BASE := -Wall -Wextra -O2
CFLAGS := $(CFLAGS_BASE) -mcpu=cortex-m3 -mthumb -nostdlib -ffreestanding
CFLAGS += -DSTM32F103xB -I$(INCLUDE_DIR) -I$(CMSIS_DIR)
CFLAGS += -I$(FREERTOS_DIR)/include -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3
LDFLAGS := -Tld/stm32f103.ld

# Source organization
SRC_SUBDIRS := app core drivers control
SRC_DIRS := $(addprefix $(SRC_DIR)/,$(SRC_SUBDIRS))
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# FreeRTOS and CMSIS sources
FREERTOS_SRC := \
    $(FREERTOS_DIR)/event_groups.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/stream_buffer.c \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
    $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c

CMSIS_SRC := \
    $(CMSIS_DIR)/startup_stm32f103xb.s \
    $(CMSIS_DIR)/system_stm32f1xx.c

ALL_SRCS := $(SRCS) $(FREERTOS_SRC) $(CMSIS_SRC)

# Object files
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(ALL_SRCS)))
OBJS += $(patsubst %.s,$(BUILD_DIR)/%.o,$(filter %.s,$(ALL_SRCS)))

# Output binary
TARGET := $(BIN_DIR)/$(PROJECT)

# Default build target
all: $(BIN_DIR) $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Compilation for .c files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation for .s files
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) -mcpu=cortex-m3 -mthumb -c $< -o $@

# Output dirs
$(BIN_DIR):
	@mkdir -p $@

# Cleanup
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
