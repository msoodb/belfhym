
To use #include "stm32f1xx.h" without errors, you need to install the STM32 HAL/LL driver headers 
on your system and set up your project to find them. Here's how to do that:

1. Download STM32CubeF1 HAL package
git clone https://github.com/STMicroelectronics/STM32CubeF1.git
Or download directly from:
https://github.com/STMicroelectronics/STM32CubeF1

2. Locate stm32f1xx.h
STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h
STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_gpio.h

3. Add the include paths to your Makefile
Add these to your CFLAGS:
```
CFLAGS += -I/path/to/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS += -I/path/to/STM32CubeF1/Drivers/CMSIS/Include
CFLAGS += -I/path/to/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc
```

4. Compile with the correct ARM toolchain
Use arm-none-eabi-gcc, not regular gcc.
```sh
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib
```

5. Then compile with:
```sh
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -o your_program.elf your_source.c $(CFLAGS)
```
