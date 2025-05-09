#!/bin/bash

mkdir -p FreeRTOS/portable/GCC/ARM_CM3
mkdir -p FreeRTOS/include

cp ~/Projects/FreeRTOS-Kernel/*.c                    FreeRTOS/
cp ~/Projects/FreeRTOS-Kernel/include/*.h            FreeRTOS/include/
cp ~/Projects/FreeRTOS-Kernel/portable/GCC/ARM_CM3/* FreeRTOS/portable/GCC/ARM_CM3/

echo "FreeRTOS setup complete."
