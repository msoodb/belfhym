# MoonRover (formerly Belfhym)

**MoonRover** is a rugged, autonomous planetary rover designed for remote exploration, scientific payload delivery, and mobility over uneven terrain. Built using embedded C on the STM32F103C8T6 microcontroller, this rover project aims to combine precise control, real-time responsiveness, and modular design for educational and experimental missions.

> *"Forged for silence, MoonRover crawls across the unknown, sensing, learning, and enduring."*

---

## Features

- Real-time multitasking with FreeRTOS  
- Modular embedded C architecture  
- Sensor integration (IMU, ultrasonic, encoders)  
- PID-based navigation and motor control  
- UART-based telemetry and debugging  
- Expandable interfaces for GPS, RF, BLE  
- Power-efficient rover firmware for STM32  

---

## Hardware Requirements

| Component       | Recommendation / Example       |
|----------------|---------------------------------|
| MCU            | STM32F103C8T6 (Blue Pill)       |
| IMU            | MPU6050, ICM20948                |
| Motors         | 2× or 4× DC motors + drivers     |
| Power          | 1S/2S Li-ion or LiPo battery     |
| Sensors        | Ultrasonic, IR, wheel encoders   |
| Comms          | NRF24L01+, UART/BLE module       |
| Frame          | Custom chassis / 3D-printed      |

---

## Project Structure
moonrover/
├── Makefile
├── ld/
├── CMSIS/
├── FreeRTOS/
├── include/
├── src/
├── tests/
└── tools/

---

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/msoodb/belfhym.git
cd belfhym
```

### 2. Build and Flash

```bash
make
make flash
```


### 3. Development Environment (Fedora Linux)

- Install toolchain:
```bash
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib
```

- Check version:
```bash
arm-none-eabi-gcc --version
```

- Enable Bear for compilation database:
```bash
bear -- make
```

- Build for ARM:
```bash
make TARGET_ARCH=arm
```

### License
This project is licensed under the GNU General Public License v3. See the LICENSE file for details.
