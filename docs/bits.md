──────────────────────────────────────────────
 EMBEDDED C BIT MANIPULATION CHEAT SHEET
──────────────────────────────────────────────

This guide covers **all** common bit operations used in microcontroller (MCU) code, 
especially when working with hardware registers, GPIO, timers, and peripherals.

──────────────────────────────────────────────
 Set a Bit
──────────────────────────────────────────────
Set bit n to 1.

    reg |= (1 << n);

Example:
    GPIOC->ODR |= (1 << 13);   // Set PC13 high (output data register)

──────────────────────────────────────────────
 Clear a Bit
──────────────────────────────────────────────
Clear bit n to 0.

    reg &= ~(1 << n);

Example:
    GPIOC->ODR &= ~(1 << 13);  // Set PC13 low

──────────────────────────────────────────────
 Toggle a Bit
──────────────────────────────────────────────
Flip bit n (0 → 1, 1 → 0).

    reg ^= (1 << n);

Example:
    GPIOC->ODR ^= (1 << 13);   // Toggle PC13

──────────────────────────────────────────────
 Check if a Bit is Set
──────────────────────────────────────────────
Check if bit n is 1.

    if (reg & (1 << n)) {
        // bit is set
    }

Example:
    if (GPIOC->IDR & (1 << 0)) {
        // PC0 input is high
    }

──────────────────────────────────────────────
 Check if a Bit is Clear
──────────────────────────────────────────────
Check if bit n is 0.

    if (!(reg & (1 << n))) {
        // bit is clear
    }

──────────────────────────────────────────────
 Set a Bit Field (Multi-Bit)
──────────────────────────────────────────────
Clear and set a multi-bit field.

    reg &= ~(mask << shift);              // clear field
    reg |= (new_value & mask) << shift;   // set new value

Example (set PLL multiplier to 9, bits 18:21):
    RCC->CFGR &= ~(0xF << 18);
    RCC->CFGR |= (0x7 << 18);  // 0x7 = PLL x9

──────────────────────────────────────────────
 Write Whole Register
──────────────────────────────────────────────
Replace entire register value.

    reg = new_value;

Example:
    GPIOC->ODR = 0x0000FFFF;   // Set lower 16 pins high

──────────────────────────────────────────────
 Read Whole Register
──────────────────────────────────────────────
Read current register value.

    uint32_t val = reg;

──────────────────────────────────────────────
9️⃣ Mask and Extract Bits
──────────────────────────────────────────────
Keep only selected bits.

    masked = reg & mask;

Example (get low nibble):
    uint8_t low_nibble = value & 0x0F;

──────────────────────────────────────────────
 Additional Tricks
──────────────────────────────────────────────
 Shift Left
    reg << n

 Shift Right
    reg >> n

 Combine Bits
    reg = (val1 << shift1) | (val2 << shift2);

 Clear All Bits
    reg = 0;

──────────────────────────────────────────────
 Helper Macros (Reusable)
──────────────────────────────────────────────
#define SET_BIT(REG, BIT)       ((REG) |= (1U << (BIT)))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(1U << (BIT)))
#define TOGGLE_BIT(REG, BIT)    ((REG) ^= (1U << (BIT)))
#define READ_BIT(REG, BIT)      (((REG) >> (BIT)) & 1U)
#define WRITE_FIELD(REG, MASK, SHIFT, VAL) \
    do { (REG) = ((REG) & ~(MASK << SHIFT)) | ((VAL & MASK) << SHIFT); } while(0)

──────────────────────────────────────────────
 Practical MCU Examples
──────────────────────────────────────────────
 Enable GPIO Port Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

 Configure GPIO Pin as Output
    GPIOC->CRH &= ~(0xF << (4 * (13 - 8)));   // clear mode/cnf
    GPIOC->CRH |=  (0x2 << (4 * (13 - 8)));   // set MODE: output 2MHz

 Set Timer Prescaler
    TIM2->PSC = 72 - 1;

 Check Interrupt Flag and Clear
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;  // clear pending flag
    }

──────────────────────────────────────────────
 Tips
──────────────────────────────────────────────
 Always use `|=` to set bits safely.
 Always use `&= ~()` to clear bits safely.
 Avoid direct `=` unless you intend to overwrite everything.
 Define helper macros to keep code clean and maintainable.

──────────────────────────────────────────────
 Want me to generate ready-to-use macros or inline functions for your STM32 project?
Just ask: I can tailor them to your codebase!
──────────────────────────────────────────────
