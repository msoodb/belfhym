.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global Reset_Handler
.global _estack

/* Define the top of stack */
_estack = 0x20005000  /* 20 KB RAM for STM32F103C8 */

.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

/* Dummy handlers */
.thumb_func
Default_Handler:
Infinite_Loop:
  b Infinite_Loop

.weak NMI_Handler
.set NMI_Handler, Default_Handler
.weak HardFault_Handler
.set HardFault_Handler, Default_Handler
.weak MemManage_Handler
.set MemManage_Handler, Default_Handler
.weak BusFault_Handler
.set BusFault_Handler, Default_Handler
.weak UsageFault_Handler
.set UsageFault_Handler, Default_Handler
.weak SVC_Handler
.set SVC_Handler, Default_Handler
.weak DebugMon_Handler
.set DebugMon_Handler, Default_Handler
.weak PendSV_Handler
.set PendSV_Handler, Default_Handler
.weak SysTick_Handler
.set SysTick_Handler, Default_Handler

/* Reset Handler */
.section .text.Reset_Handler
.thumb_func
.type Reset_Handler, %function
Reset_Handler:
  /* Set up stack and initialize data/bss. Then call main. */

  ldr r0, =_sidata
  ldr r1, =_sdata
  ldr r2, =_edata
1:
  cmp r1, r2
  ittt lt
  ldrlt r3, [r0], #4
  strlt r3, [r1], #4
  blt 1b

  ldr r0, =_sbss
  ldr r1, =_ebss
  movs r2, #0
2:
  cmp r0, r1
  it lt
  strlt r2, [r0], #4
  blt 2b

  bl main
  b .
