# Belfhym: STM32F103C8T6 Embedded Template

**Belfhym** is a streamlined embedded project template for **STM32F103C8T6 (Blue Pill)** microcontrollers, featuring:

- **FreeRTOS** real-time operating system
- **CMSIS** bare-metal targeting (no HAL)
- Clean modular architecture
- Template-ready for copy-paste usage
- Minimal resource footprint (21,584 bytes)

This template provides a solid foundation for embedded projects with essential components: motors, sensors, OLED display, and basic I/O control.

---

## Current Features

### Hardware Components
- **Motors**: Dual DC motor control (L298N compatible)
- **Servo**: Single servo motor control (PWM)
- **Sensors**: Ultrasonic distance sensor (HC-SR04), IR remote, mode button
- **Display**: OLED display (I2C, SSD1306 compatible)
- **LEDs**: Onboard LED, external LED, debug LED
- **Communication**: I2C protocol for display

### Software Architecture
- **Task-based design** with FreeRTOS
- **Hub pattern**: Sensor Hub → Controller → Actuator Hub
- **Modular structure** with clear separation of concerns
- **Template-friendly**: No conditional compilation (#ifdef guards)

---

## Project Structure

```
src/
├── actuators/          # Output devices
│   ├── blfm_actuator_hub.c
│   ├── blfm_led.c
│   ├── blfm_motor.c
│   ├── blfm_oled.c
│   └── blfm_servomotor.c
├── logic/              # Control logic
│   └── blfm_controller.c
├── core/               # System core
│   ├── blfm_board.c
│   ├── blfm_clock.c
│   ├── blfm_exti_dispatcher.c
│   ├── blfm_taskmanager.c
│   └── libc_stubs.c
├── drivers/            # Hardware drivers
│   ├── blfm_gpio.c
│   └── blfm_pwm.c
├── protocols/          # Communication protocols
│   └── blfm_i2c.c
├── sensors/            # Input devices
│   ├── blfm_sensor_hub.c
│   ├── blfm_ultrasonic.c
│   ├── blfm_ir_remote.c
│   └── blfm_mode_button.c
├── utils/              # Utilities
│   ├── blfm_debug.c
│   ├── blfm_delay.c
│   └── blfm_font8x8.c
└── belfhym.c           # Main entry point
```

---

## Task Architecture

| Task Name | Responsibility | Priority | Communication |
|-----------|---------------|----------|---------------|
| **SensorTask** | Polls ultrasonic sensor, handles inputs | Medium | Sends data via queues |
| **ControllerTask** | Processes sensor data, makes decisions | High | Receives from sensor, sends to actuators |
| **ActuatorTask** | Controls motors, LEDs, display, servo | High | Receives commands via queues |
| **IRTask** | Handles IR remote commands | Medium | Interrupt-driven |
| **ButtonTask** | Handles mode button presses | Low | Interrupt-driven |

## Communication Flow

```
[Sensors] → [Sensor Hub] → [Controller] → [Actuator Hub] → [Hardware]
    ↓           ↓             ↓              ↓
[Ultrasonic] [Data Queue] [Logic/PID] [LED/Motor/OLED]
[IR Remote]
[Button]
```

---

## Hardware Requirements

| Component | Description | Pins Used |
|-----------|-------------|-----------|
| **MCU** | STM32F103C8T6 (Blue Pill) | - |
| **Motors** | Dual DC motors with L298N driver | PA0, PA1, PB0, PB1, PB10, PB11 |
| **Servo** | Standard servo motor | PA7 (TIM3_CH2) |
| **Ultrasonic** | HC-SR04 distance sensor | PB3 (Echo), PB4 (Trigger) |
| **OLED** | SSD1306 128x64 I2C display | PB6 (SCL), PB7 (SDA) |
| **IR Remote** | IR receiver module | PA8 |
| **Button** | Mode selection button | PA4 |
| **LEDs** | Status indicators | PC13 (onboard), PB5 (external), PC15 (debug) |

---

## Getting Started

### Prerequisites

```bash
# Fedora/RHEL
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib

# Ubuntu/Debian
sudo apt install gcc-arm-none-eabi

# Verify installation
arm-none-eabi-gcc --version
```

### Build and Flash

```bash
# Clone and build
git clone https://github.com/msoodb/belfhym.git
cd belfhym
make

# Flash to device (multiple methods available)
make flash                    # Default: ST-Link
make METHOD=openocd flash     # OpenOCD
make METHOD=dfu flash         # DFU mode
```

### Flash Methods

```bash
make METHOD=stlink flash      # ST-Link (default)
make METHOD=openocd flash     # OpenOCD
make METHOD=gdb flash         # GDB with OpenOCD
make METHOD=serial flash      # Serial bootloader
make METHOD=dfu flash         # DFU mode
```

---

## Using as Template

1. **Copy the project**: `cp -r belfhym my_new_project`
2. **Rename**: Update `PROJECT` name in `Makefile`
3. **Customize**: Add/remove components as needed
4. **Build**: `make clean && make`

### Template Benefits

- **No configuration overhead**: No #ifdef guards or config files
- **Modular design**: Easy to add/remove components
- **Clean structure**: Logical separation of concerns
- **Ready-to-use**: Compile and flash immediately
- **Minimal footprint**: Optimized for STM32F103C8T6's limited resources

---

## Binary Size

Current template size: **21,584 bytes** (Flash) + **17,920 bytes** (RAM)

---

## License

This project is licensed under the GNU General Public License v3. See the LICENSE file for details.

---

## Contributing

This template is designed to be a starting point for embedded projects. Feel free to fork and adapt for your specific needs.