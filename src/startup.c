#include <stdint.h>

// Linker script symbols
extern uint32_t _estack;
extern int main(void);

void Reset_Handler(void);
void Default_Handler(void);

// Vector table (put in .isr_vector section)
__attribute__ ((section(".isr_vector")))
void (* const vector_table[])(void) = {
    (void (*)(void))(&_estack), // Initial Stack Pointer
    Reset_Handler,              // Reset Handler
    Default_Handler,            // NMI Handler
    Default_Handler,            // Hard Fault Handler
    Default_Handler,            // Memory Management Fault Handler
    Default_Handler,            // Bus Fault Handler
    Default_Handler,            // Usage Fault Handler
    0, 0, 0, 0,                 // Reserved
    Default_Handler,            // SVCall Handler
    Default_Handler,            // Debug Monitor Handler
    0,                          // Reserved
    Default_Handler,            // PendSV Handler
    Default_Handler             // SysTick Handler
    // Add peripheral IRQ handlers here if needed
};

void Reset_Handler(void) {
    extern uint32_t _sdata, _edata, _sidata;
    extern uint32_t _sbss, _ebss;

    // Copy .data from Flash to RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *(dst++) = *(src++);

    // Zero .bss
    dst = &_sbss;
    while (dst < &_ebss)
        *(dst++) = 0;

    // Call main
    main();

    // Trap if main returns
    while (1);
}

void Default_Handler(void) {
    while (1);
}
