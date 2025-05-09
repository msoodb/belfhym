```sh
git clone --recurse-submodules https://github.com/ARM-software/CMSIS_5.git
cd CMSIS_5
git submodule update --init --recursive

cd CMSIS_5/CMSIS/Device
git submodule add https://github.com/STMicroelectronics/cmsis_device_f1.git ST/STM32F1xx
git submodule update --init --recursive


```
