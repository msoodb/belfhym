#!/bin/bash

# Source and destination
CMSIS_SRC=~/Projects/CMSIS_5
DEST=~/Projects/belfhym/CMSIS
DEVICE_DEST="$DEST/device"

# Create destination folders
mkdir -p "$DEVICE_DEST"

# Files to copy
FILES_CORE=(
    "$CMSIS_SRC/CMSIS/Core/Include/core_cm3.h"
)

FILES_DEVICE=(
    "$CMSIS_SRC/CMSIS/Device/ST/STM32F1xx/Include/system_stm32f1xx.h"
    "$CMSIS_SRC/CMSIS/Device/ST/STM32F1xx/Include/stm32f100xb.h"
    "$CMSIS_SRC/CMSIS/Device/ST/STM32F1xx/Include/stm32f107xc.h"
    "$CMSIS_SRC/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c"
)

# Copy core files
for file in "${FILES_CORE[@]}"; do
    if [[ -f "$file" ]]; then
        cp "$file" "$DEST/"
        echo "Copied $(basename "$file")"
    else
        echo "Missing: $file"
    fi
done

# Copy device-specific files
for file in "${FILES_DEVICE[@]}"; do
    if [[ -f "$file" ]]; then
        cp "$file" "$DEVICE_DEST/"
        echo "Copied $(basename "$file")"
    else
        echo "Missing: $file"
    fi
done

echo "CMSIS setup complete in: $DEST"
