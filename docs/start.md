I want to start a project belfhym with C language on Linux Fedora 42 machine. the goal is to create a fying drone like robot. I want to use FreeRTOS and STM32F103 microcontroller. 

create a minimal project that support all requirements and can be build and test on my laptop before deployment, the first version should be minimal and just be hello world. 
The structure in my mind is 

belfhym/
│
├── Makefile               
├── README.md
├── src/              
├── include/
├── startup/
├── ld/ 
├── CMSIS/                 # CMSIS headers for STM32F1
└── FreeRTOS/              # FreeRTOS kernel source

It should contain all needed file like in src and include. also give me a list of file should be added to FreeRTOS and CMSIS folder, I will copy them,

the main.c should be changed to   belfhym.c. 
