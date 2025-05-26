#!/bin/bash

set -e  # Exit on any error

# --- CONFIG ---
PROJECT_GIT="https://github.com/msoodb/belfhym.git"
PROJECT_DIR="$HOME/belfhym"
STM32CUBE_REPO="https://github.com/STMicroelectronics/STM32CubeF1.git"
CMSIS_REPO="https://github.com/ARM-software/CMSIS_5.git"
CMSIS_DEVICE_REPO="https://github.com/STMicroelectronics/cmsis_device_f1.git"

echo "Setting up Belfhym project..."

# --- Step 1: Install system packages ---
echo "Installing required packages..."
sudo dnf install -y git make arm-none-eabi-gcc-cs arm-none-eabi-newlib arm-none-eabi-gdb openocd

# --- Step 2: Clone Belfhym project ---
if [ ! -d "$PROJECT_DIR" ]; then
    echo "Cloning Belfhym project..."
    git clone "$PROJECT_GIT" "$PROJECT_DIR"
else
    echo "Belfhym project already exists at $PROJECT_DIR"
fi

# --- Step 3: Clone STM32CubeF1 HAL package ---
if [ ! -d "$PROJECT_DIR/STM32CubeF1" ]; then
    echo "Cloning STM32CubeF1 HAL package..."
    git clone "$STM32CUBE_REPO" "$PROJECT_DIR/STM32CubeF1"
else
    echo "STM32CubeF1 already exists"
fi

# --- Step 4: Clone CMSIS (ARM core support) ---
if [ ! -d "$PROJECT_DIR/CMSIS_5" ]; then
    echo "Cloning CMSIS_5..."
    git clone --recurse-submodules "$CMSIS_REPO" "$PROJECT_DIR/CMSIS_5"
    cd "$PROJECT_DIR/CMSIS_5/CMSIS/Device"
    git submodule add "$CMSIS_DEVICE_REPO" ST/STM32F1xx
    git submodule update --init --recursive
    cd "$PROJECT_DIR"
else
    echo "CMSIS_5 already exists"
fi

# --- Step 5: Set up include paths (if using Makefile) ---
echo "Make sure your Makefile has these include paths:"
echo "   -I$PROJECT_DIR/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include"
echo "   -I$PROJECT_DIR/STM32CubeF1/Drivers/CMSIS/Include"
echo "   -I$PROJECT_DIR/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc"

# --- Done ---
echo "Setup complete!"
echo "You can now build your project by running:"
echo "    cd $PROJECT_DIR"
echo "    make"
echo "Or manually compile with:"
echo "    arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -o output.elf source.c <CFLAGS>"
