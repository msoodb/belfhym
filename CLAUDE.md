# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build the project
make

# Build and flash to device
make flash

# Flash only (after building)
make deploy

# Clean build artifacts
make clean

# Check binary size
make size
```

### Flash Methods
Multiple deployment methods are available via the `METHOD` variable:
- `make METHOD=stlink flash` (default)
- `make METHOD=openocd flash`
- `make METHOD=gdb flash`
- `make METHOD=serial flash`
- `make METHOD=dfu flash`

## Project Architecture

**Belfhym** is a streamlined embedded template for STM32F103C8T6 (Blue Pill) with a clean, modular architecture optimized for template usage.

### Core Structure

1. **Entry Point**: `src/belfhym.c` - Main function that initializes board and starts task manager
2. **Task Management**: `src/core/blfm_taskmanager.c` - Sets up and manages all FreeRTOS tasks
3. **Board Initialization**: `src/core/blfm_board.c` - Hardware setup and peripheral initialization

### Module Organization

The codebase is organized into logical modules under `src/`:

- **actuators/**: Output devices (motors, LEDs, servo, OLED display)
- **sensors/**: Input devices (ultrasonic, IR remote, mode button)
- **logic/**: Control logic and decision making
- **core/**: Board setup, clocks, task management, interrupt dispatcher
- **drivers/**: Low-level hardware drivers (GPIO, PWM)
- **protocols/**: Communication protocols (I2C)
- **utils/**: Utility functions (debug, delay, font)

### Current Components

**Active Components:**
- **Motors**: Dual DC motor control with L298N driver
- **Servo**: Single servo motor (PWM-based)
- **Ultrasonic**: HC-SR04 distance sensor
- **OLED**: SSD1306 128x64 I2C display
- **IR Remote**: IR receiver for remote control
- **Mode Button**: Physical button for mode switching
- **LEDs**: Onboard, external, and debug LEDs

**Protocols:**
- **I2C**: Simple I2C1-only implementation for OLED display
- **GPIO**: Pin control for sensors and actuators
- **PWM**: Timer-based PWM for servo control

### Task Communication Pattern

The system follows a streamlined hub-based architecture:
- **Sensor Hub**: Aggregates sensor data (ultrasonic readings)
- **Controller**: Processes sensor data and makes control decisions
- **Actuator Hub**: Routes commands to motors, LEDs, servo, and display

Tasks communicate via FreeRTOS primitives (queues, notifications) with minimal overhead.

### Hardware Target

- **MCU**: STM32F103C8T6 (Cortex-M3, 72MHz, 20KB RAM)
- **RTOS**: FreeRTOS with CMSIS (no HAL)
- **Toolchain**: arm-none-eabi-gcc
- **Linker Script**: `ld/stm32f103.ld`
- **Binary Size**: 21,584 bytes (Flash) + 17,920 bytes (RAM)

### Template-Specific Features

- **No Configuration System**: No blfm_config.h or #ifdef guards
- **Copy-Paste Ready**: Clean code without conditional compilation
- **Minimal Footprint**: Optimized for STM32F103C8T6's limited resources
- **Modular Design**: Easy to add/remove components
- **Clean Architecture**: Logical separation of concerns

### Pin Assignments

| Component | Port/Pin | Description |
|-----------|----------|-------------|
| Motor Left EN | PA0 | Left motor enable |
| Motor Right EN | PA1 | Right motor enable |
| Mode Button | PA4 | Mode selection button |
| Servo PWM | PA7 | Servo control (TIM3_CH2) |
| IR Remote | PA8 | IR receiver input |
| Motor Left IN1 | PB0 | Left motor direction 1 |
| Motor Left IN2 | PB1 | Left motor direction 2 |
| Ultrasonic Echo | PB3 | Echo pin |
| Ultrasonic Trig | PB4 | Trigger pin |
| LED External | PB5 | External LED |
| I2C SCL | PB6 | I2C clock |
| I2C SDA | PB7 | I2C data |
| Motor Right IN1 | PB10 | Right motor direction 1 |
| Motor Right IN2 | PB11 | Right motor direction 2 |
| LED Onboard | PC13 | Blue Pill onboard LED |
| LED Debug | PC15 | Debug status LED |

### Development Notes

- The system uses bare-metal CMSIS without STM32 HAL for direct hardware control
- All source files include GPL-3.0 license headers
- The build system automatically includes all `.c` files from the module directories
- No configuration flags - all components are built and available

## Template Usage Guidelines

When using Belfhym as a template:

### Adding New Components
1. Create source file in appropriate module directory
2. Add corresponding header file in `include/`
3. Add pin definitions to `include/blfm_pins.h`
4. Initialize component in appropriate hub (sensor/actuator)
5. Add task if needed in `blfm_taskmanager.c`

### Removing Components
1. Remove source and header files
2. Remove pin definitions from `blfm_pins.h`
3. Remove initialization calls from hubs
4. Remove task creation from task manager
5. Update type definitions in `blfm_types.h`

### Code Quality Standards
- Maintain consistent `blfm_` prefixing for all functions and types
- Use symbolic constants instead of magic numbers
- Implement proper resource cleanup functions (`*_deinit()`) for all modules
- Add comprehensive function documentation with parameters, return values, and usage notes

### Hardware Abstraction
- All GPIO operations should go through `blfm_gpio.h` abstraction
- Avoid direct register access in high-level modules
- Maintain clear separation between driver and application layers
- Use consistent peripheral initialization patterns

### Error Handling
- Use standardized return codes for consistency
- Add null pointer validation at function entry points
- Implement proper error propagation through the task communication chain
- Consider watchdog integration for autonomous operation safety

### Memory Optimization
- Large structs should use unions for optional components
- Add timestamping to sensor data for temporal analysis
- Consider selective updates rather than full struct copying through queues
- Stack allocation preferred over dynamic allocation

### Task Communication
- Use FreeRTOS queues for data transfer between tasks
- Implement command validation and bounds checking
- Use priority-based task scheduling for real-time requirements
- Add queue overflow handling and recovery mechanisms

## Template Benefits

1. **Immediate Usability**: Compile and flash without configuration
2. **Modular Design**: Easy to add/remove components
3. **Clean Architecture**: Logical separation of concerns
4. **Minimal Footprint**: Optimized for resource-constrained MCUs
5. **Template-Friendly**: No conditional compilation overhead
6. **Well-Documented**: Clear structure and usage guidelines