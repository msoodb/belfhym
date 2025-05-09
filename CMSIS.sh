#!/bin/bash

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 Masoud Bolhassani

CMSIS_CORE=~/Projects/CMSIS_5/CMSIS/Core
CMSIS_DEVICE=~/Projects/CMSIS_5/CMSIS/Device/ST/STM32F1xx
DEST=CMSIS

mkdir -p "$DEST"

cp "$CMSIS_CORE/Include/core_cm3.h" "$DEST/"
cp "$CMSIS_CORE/Include/cmsis_compiler.h" "$DEST/"
cp "$CMSIS_CORE/Include/cmsis_gcc.h" "$DEST/"
cp "$CMSIS_CORE/Include/cmsis_version.h" "$DEST/"

cp "$CMSIS_DEVICE/Include/system_stm32f1xx.h" "$DEST/"
cp "$CMSIS_DEVICE/Include/stm32f103xb.h" "$DEST/"
cp "$CMSIS_DEVICE/Include/stm32f1xx.h" "$DEST/"

cp "$CMSIS_DEVICE/Source/Templates/system_stm32f1xx.c" "$DEST/"
cp "$CMSIS_DEVICE/Source/Templates/gcc/startup_stm32f103xb.s" "$DEST/"

echo "CMSIS setup complete in: $DEST"
