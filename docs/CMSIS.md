To add CMSIS to your STM32F103 + FreeRTOS project properly, follow these steps:
✅ 1. Download CMSIS and STM32F1 Device Headers

You can get CMSIS in two main ways:
Option A: Using STM32CubeF1 (Recommended for STM32F103)

    Install STM32CubeF1:

        Download from ST: STM32CubeF1

        Unzip it.

    Copy CMSIS & device files:
    From the CubeF1 package:

    STM32Cube_FW_F1_VX.XX.X/Drivers/CMSIS/Core/Include/               → belfhym/CMSIS/
    STM32Cube_FW_F1_VX.XX.X/Drivers/CMSIS/Device/ST/STM32F1xx/Include → belfhym/CMSIS/device/
    STM32Cube_FW_F1_VX.XX.X/Drivers/CMSIS/Device/ST/STM32F1xx/Source  → belfhym/CMSIS/device/

Option B: Use ARM CMSIS Pack

    Download from: https://github.com/ARM-software/CMSIS_5

    Use only CMSIS/Core and then manually get STM32 device headers (more work than Option A).

✅ 2. Add to Include Paths in Makefile

In your Makefile, add the following include paths:

INCLUDES = \
  -Iinc \
  -Isrc \
  -ICMSIS \
  -ICMSIS/device \
  -IFreeRTOS/include \
  -IFreeRTOS/portable/GCC/ARM_CM3

✅ 3. Use CMSIS in Your Code

You can now do:

#include "stm32f10x.h"

And safely use macros like:

RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

