
mkdir -p freertos/portable/GCC/ARM_CM3
mkdir -p freertos/include

cp ~/Projects/FreeRTOS-Kernel/*.c                    freertos/
cp ~/Projects/FreeRTOS-Kernel/include/*.h            freertos/include/
cp ~/Projects/FreeRTOS-Kernel/portable/GCC/ARM_CM3/* freertos/portable/GCC/ARM_CM3/
