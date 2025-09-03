# Belfhym Tactical Battle Rover

Belfhym is a FreeRTOS-based tactical battle rover control system for the STM32F103C8T6 (Blue Pill) microcontroller, implementing differential drive motor control, servo management, and multi-sensor fusion with S17 wireless communication.

## Project Overview

- **Purpose**: Remote-controlled tactical battle rover with autonomous capabilities
- **MCU**: STM32F103C8T6 (Blue Pill)
- **RTOS**: FreeRTOS
- **Communication**: S17 protocol over NRF24L01+
- **Device ID**: `S17_TN_BELFHYM` (0x2002)

## Key Features

- Differential drive motor control (PWM with H-bridge)
- 4-channel servo control
- Multiple input sources (S17 wireless, IR remote, button)
- Three operating modes (Manual, Auto, Emergency)
- Ultrasonic obstacle avoidance
- Real-time telemetry

## Directory Structure

```
belfhym/
├── src/               # Source files
│   ├── board/        # Board initialization
│   ├── controls/     # Controller logic
│   ├── devices/      # Device drivers
│   ├── drivers/      # Low-level drivers
│   ├── system/       # System management
│   └── utils/        # Utility functions
├── include/          # Header files
├── FreeRTOS/        # RTOS kernel
├── CMSIS/           # ARM Cortex-M3 support
├── ld/              # Linker scripts
└── S17/             # Wireless communication (submodule)
```

## Building

```bash
# Build the firmware
make

# Clean build artifacts
make clean

# Flash to device
make flash
```

## Hardware Configuration

### Motor Control
- Left Motor: PWM on PB6, Direction on PB8/PB9
- Right Motor: PWM on PB7, Direction on PB0/PB1

### Servo Control
- Servo 1-4: PA8-PA11 (TIM1 channels)

### Communication
- NRF24L01+: SPI1 with CE=PA4, CSN=PA0

### Sensors
- Ultrasonic: Trigger=PB12, Echo=PB11
- IR Remote: PA2
- Button: PA3

## S17 Network Configuration

The S17 wireless module should be added as a git submodule:

```bash
git submodule add <S17-repo-url> S17
git submodule update --init
```

## Control Modes

1. **Manual Mode**: Remote control via S17 or IR remote
2. **Auto Mode**: Autonomous obstacle avoidance
3. **Emergency Mode**: All actuators stop

## Differences from Homa

- All prefixes changed from `homa_`/`HOMA_` to `blfm_`/`BLFM_`
- Device ID: `S17_TN_BELFHYM` instead of `S17_TN_HOMA`
- Configured as a separate robot in the S17 network

## License

Copyright (C) 2025 Masoud Bolhassani