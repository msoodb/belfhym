# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 Masoud Bolhassani

PROJECT := belfhym
FREERTOS_DIR = FreeRTOS
CMSIS_DIR = CMSIS
CFLAGS_BASE = -Wall -Wextra -O2
HEADERS_PATH = -I./include -I$(CMSIS_DIR) -I$(FREERTOS_DIR)/include -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3

# Architecture-dependent compiler and flags
ifeq ($(TARGET_ARCH),arm)
CC = arm-none-eabi-gcc
CFLAGS = $(CFLAGS_BASE) -mcpu=cortex-m3 -mthumb -nostdlib -ffreestanding $(HEADERS_PATH)
CFLAGS += -DSTM32F103xB
LDFLAGS = -Tld/stm32f103.ld
else
CC = gcc
CFLAGS = $(CFLAGS_BASE) -Wunused-result $(HEADERS_PATH)
LDFLAGS = -lpcap -lncurses
endif

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# FreeRTOS sources (relative to project root)
FREERTOS_SRC = \
    $(FREERTOS_DIR)/event_groups.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/stream_buffer.c \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
    $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c

# CMSIS sources (relative to project root)
CMSIS_SRC = \
    $(CMSIS_DIR)/startup_stm32f103xb.s \
    $(CMSIS_DIR)/system_stm32f1xx.c

SRC += $(FREERTOS_SRC) $(CMSIS_SRC)

# Object files (mirrored under build/)
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(subst ./,,$(SRC)))
TARGET = $(BIN_DIR)/$(PROJECT)

VERSION := $(shell grep -v '^#' VERSION | head -n 1)
TAG := v$(VERSION)
TARBALL := $(PROJECT)-$(VERSION).tar.gz
PREFIX := $(PROJECT)-$(VERSION)
SOURCEDIR := $(HOME)/rpmbuild/SOURCES
SPECDIR := $(HOME)/rpmbuild/SPECS

# Default build
all: $(BIN_DIR) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Pattern rule for object file compilation, supports nested dirs
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Version bumping script
bump:
	@read -p "Enter version bump type (patch, minor, major): " bump_type; \
	./scripts/bump-version.sh $$bump_type

# Git tag from version file
tag:
	@if git rev-parse $(TAG) >/dev/null 2>&1; then \
		echo "Tag $(TAG) already exists."; \
	else \
		git tag -a $(TAG) -m "Release $(TAG)"; \
		git push origin $(TAG); \
	fi

# Create tarball from Git tag
tarball:
	git archive --format=tar.gz --prefix=$(PREFIX)/ $(TAG) > $(SOURCEDIR)/$(TARBALL)

# Copy spec to rpmbuild
copy-spec:
	./scripts/copy-spec.sh $(SPECDIR)/

# Build RPM
rpm:
	rpmbuild -ba $(SPECDIR)/$(PROJECT).spec
	rpmbuild -bs $(SPECDIR)/$(PROJECT).spec

# Full release process
release: tag tarball copy-spec rpm

# Installation
install: install-bin

install-bin:
	install -D -m 0755 $(TARGET) $(DESTDIR)/usr/bin/$(PROJECT)

uninstall:
	rm -f $(DESTDIR)/usr/bin/$(PROJECT)
	rm -rf $(DESTDIR)/usr/share/doc/$(PROJECT)

# Clean build files
clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean install install-bin uninstall bump tag tarball copy-spec rpm release
