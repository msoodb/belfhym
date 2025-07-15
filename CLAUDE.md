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

**Belfhym** is a FreeRTOS-based lunar rover system for STM32F103C8T6 (Blue Pill) with a modular, task-oriented architecture.

### Core Structure

1. **Entry Point**: `src/app/belfhym.c` - Main function that initializes board and starts task manager
2. **Task Management**: `src/app/blfm_taskmanager.c` - Sets up and manages all FreeRTOS tasks
3. **Board Initialization**: `src/core/blfm_board.c` - Hardware setup and peripheral initialization

### Module Organization

The codebase is organized into logical modules under `src/`:

- **actuators/**: Output devices (motors, LEDs, servos, display, alarm)
- **sensors/**: Input devices (ultrasonic, IMU, temperature, IR remote, buttons)
- **communication/**: UART, radio, ESP32 interfaces
- **control/**: PID controller, pathfinding algorithms
- **core/**: Board setup, clocks, I2C, interrupt dispatcher
- **drivers/**: Low-level hardware drivers (GPIO, PWM, ADC, delays)
- **safety/**: Failsafe mechanisms and safety monitoring
- **power/**: Power management
- **monitoring/**: System monitoring and telemetry
- **logging/**: Debug and telemetry logging

### Configuration System

Module presence is controlled via compile-time flags in `include/blfm_config.h`:
- `BLFM_ENABLED_*` flags enable/disable specific sensors, actuators, and features
- Allows building minimal configurations for different hardware setups

### Task Communication Pattern

The system follows a hub-based architecture:
- **Sensor Hub**: Aggregates all sensor data
- **Controller**: Processes sensor data and makes decisions
- **Actuator Hub**: Routes commands to appropriate actuators

Tasks communicate via FreeRTOS primitives (queues, event groups, semaphores) as defined in the task manager.

### Hardware Target

- **MCU**: STM32F103C8T6 (Cortex-M3, 72MHz, 20KB RAM)
- **RTOS**: FreeRTOS with CMSIS (no HAL)
- **Toolchain**: arm-none-eabi-gcc
- **Linker Script**: `ld/stm32f103.ld`

### Development Notes

- The system uses bare-metal CMSIS without STM32 HAL for direct hardware control
- All source files include GPL-3.0 license headers
- Configuration flags in `blfm_config.h` should be checked before working with specific modules
- The build system automatically includes all `.c` files from the module directories

## Refactoring Guidelines

Based on comprehensive codebase analysis, follow these guidelines when making improvements:

### Error Handling Standards
- Use standardized error codes: `BLFM_OK`, `BLFM_ERR_NULL_PTR`, `BLFM_ERR_INVALID_PARAM`, `BLFM_ERR_TIMEOUT`, `BLFM_ERR_HARDWARE`
- All functions that can fail should return `blfm_error_t`
- Add null pointer validation at function entry points
- Implement proper error propagation through the task communication chain

### Memory Optimization
- Large structs (`blfm_sensor_data_t`, `blfm_actuator_command_t`) should use unions for optional components
- Add timestamping to sensor data for temporal analysis
- Consider selective updates rather than full struct copying through queues
- Stack allocation preferred over dynamic allocation

### Code Quality Standards
- Maintain consistent `blfm_` prefixing for all functions and types
- Use symbolic constants instead of magic numbers (define in `include/blfm_constants.h`)
- Implement proper resource cleanup functions (`*_deinit()`) for all modules
- Add comprehensive function documentation with parameters, return values, and usage notes

### Hardware Abstraction
- All GPIO operations should go through `blfm_gpio.h` abstraction
- Avoid direct register access in high-level modules
- Maintain clear separation between driver and application layers
- Use consistent peripheral initialization patterns

### Safety and Monitoring
- Implement system health monitoring for sensor/actuator failures
- Add watchdog integration for autonomous operation safety
- Implement proper failsafe mechanisms for critical operations
- Add communication timeout detection and recovery

### Task Communication
- Add bi-directional communication for actuator command feedback
- Implement command validation and bounds checking
- Use priority-based task scheduling for real-time requirements
- Add queue overflow handling and recovery mechanisms

### Configuration Management
- Centralize configuration constants in dedicated header files
- Use descriptive names for threshold values and timing parameters
- Document configuration dependencies between modules
- Maintain backward compatibility when adding new config options

### Priority Implementation Order
1. **High Priority**: Error handling standardization, memory optimization, safety systems
2. **Medium Priority**: Configuration management, hardware abstraction consistency, documentation
3. **Low Priority**: Performance optimization, testing infrastructure, advanced features