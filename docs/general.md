# GPIO (General-Purpose Input/Output)

GPIO is a fundamental feature on microcontrollers. Each GPIO pin can be configured to act as either:
    Input: To read signals (e.g., from a button or sensor).
    Output: To control devices (e.g., turn an LED or buzzer on/off).

Example use cases:
    Read a switch → input.
    Blink an LED → output.

On the STM32F103, you configure GPIO using registers like GPIOx->CRL, GPIOx->ODR, GPIOx->IDR, etc.

# PWM (Pulse Width Modulation)

PWM is a technique used to simulate analog output using digital signals by rapidly toggling a pin on/off with a specific duty cycle.
    Duty cycle = the percentage of time the signal is HIGH in each period.
    Commonly used to:
        Control motor speed
        Dim LEDs
        Control servo positions

In STM32, PWM is generated using timers (e.g., TIM2, TIM3), configured to output a square wave with adjustable duty cycle.

Purpose	            Type	Pin       config
-----------------------------------------------
LED	                GPIO   Output	On/Off state
Ultrasonic Trigger	GPIO   Output	Short pulse
Ultrasonic Echo	    GPIO   Input	Detect signal
Motor Speed	        PWM   Output	Vary speed

# HAL 
Stands for Hardware Abstraction Layer. It's a high-level API provided 
by STMicroelectronics (the maker of STM32 chips) to simplify programming their microcontrollers.

- What HAL Does:
	HAL wraps low-level hardware access in easy-to-use C functions. 
	Instead of writing directly to registers like 
	"GPIOA->ODR |= (1 << 5);" write  "HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);"

So HAL:
- Makes code more portable across STM32 families.
- Hides the complexity of register-level programming.
- Provides ready-to-use drivers for peripherals like USART, SPI, I2C, etc.

## HAL vs. CMSIS vs. Direct Register Access

Layer	        Example	                                        Description
------------------------------------------------------------------------------------------------
HAL (High)	    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);	        Easy, but adds overhead.
LL (Low Layer)	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13);	Lightweight alternative to HAL.
CMSIS	        `GPIOC->ODR	                                    = (1 << 13);`
Bare Metal	    Register manipulation	                        Fastest, smallest, but hardest to maintain.


## Why You’re Not Using HAL
In Belfhym project, chose to not use HAL for more control, 
lower overhead, and better learning of how STM32 hardware really work and directly use:

#include "stm32f1xx.h"

GPIOA->CRL = ...;
GPIOA->ODR |= (1 << 5);

This is bare-metal programming using CMSIS headers only.

Use Case	                     Recommendation
-----------------------------------------------------
Fast dev, portability	         HAL
Maximum efficiency, control	     Direct + CMSIS
Critical timing or safety	     CMSIS or Bare Metal
Balanced project	             Mix of HAL and Direct

