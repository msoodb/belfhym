# Belfhym: FreeRTOS-Based Lunar Rover Robot

Belfhym is a modular, real-time lunar rover robot powered by FreeRTOS and CMSIS, designed for autonomous terrain navigation, 
multi-sensor fusion, real-time telemetry, and manual override via remote command.
Built for the STM32F103C8T6 (Blue Pill), Belfhym integrates multiple control and sensing subsystems into a clean, 
multitasking environment with well-defined inter-task communication and scheduling policies.

---

## Project Highlights

- 6-Wheel Motor Control with PID regulation
- Multi-Sensor Fusion: Ultrasonic, IMU, thermal, power, radio, etc.
- Failsafe and Safety System
- Real-Time Telemetry over UART/radio
- Manual Control Mode
- FreeRTOS Scheduling with queues, semaphores, and notifications
- CMSIS Bare-Metal Targeting (no HAL, clean hardware control)
- Well-Structured Codebase with modular architecture and make-based build system
---


## Software Design Overview

1. Task Architecture

| Task Name         | Responsibility                               | Priority | Communication                    |
| ----------------- | -------------------------------------------- | -------- | -------------------------------- |
| **SensorTask**    | Polls ultrasonic, IMU, thermal sensors       | Med      | Sends readings via **queues**    |
| **MotorTask**     | Drives motors using PID control              | High     | Gets target direction from queue |
| **PathFinding**   | Receives sensor input, computes direction    | Med      | Queue input/output               |
| **SafetyTask**    | Monitors danger conditions (thermal, power)  | High     | Uses **event group + notify**    |
| **FailsafeTask**  | Triggers alarm, stops motors in emergency    | High     | Reacts to event bits             |
| **CommTask**      | Handles telemetry and manual command input   | Low      | **Queue + semaphore** for UART   |
| **ManualControl** | Overrides auto pathing based on command mode | Med      | Waits on a **binary semaphore**  |
| **LED/DebugTask** | Visual debug with LED pattern/status blink   | Low      | Receives status messages         |


2. Inter-Task Communication

- SensorTask → PathFinding: via Queue<SensorPacket>
- PathFinding → MotorTask: via Queue<DriveCommand>
- Safety events (thermal, power): via EventGroup or TaskNotify
- Manual control trigger: via BinarySemaphore or mode bit flag
- CommTask → ManualControl: via Queue<Command> + mode flag
- Telemetry logging: via MessageBuffer or streaming queue

                        +--------------------+
                        |     Scheduler      |  <-- FreeRTOS (manages tasks)
                        +---------+----------+
                                  |
                                  v
                          +---------------+
                          |  belfhym.c    |  <-- Entry point
                          +-------+-------+
                                  |
                                  v
                          +---------------+
                          |   BrainTask   |  <-- Core decision-making hub
                          +-------+-------+
         -----------------/       |        \-------------------------
        /                         |                                   \
       v                          v                                    v
+------------------+   +------------------+                 +------------------+
| UltrasonicTask   |   |     IMUTask      |                 |  ManualControl   |
+------------------+   +------------------+                 +------------------+
       |                          |                                   |
       |                          |                                   v
       |                          \----------------> +------------------+
       |                                               |    CommTask     |
       |                                               +--------+--------+
       |                                                        |
       v                                                        v
+------------------+                                 +------------------+
|   SafetyTask     |                                 |  FailsafeTask    |
+--------+---------+                                 +------------------+
         |
         v
+------------------+       +------------------+       +------------------+
|  PathFinding     | ----> |   MotorTask      | ----> |   LED/DebugTask  |
+------------------+       +------------------+       +------------------+



## Hardware Requirements

| Component             | Description                                              |
| --------------------- | -------------------------------------------------------- |
| **MCU**               | STM32F103C8T6 (Blue Pill) – Cortex-M3, 72 MHz, 20 KB RAM |
| **Motors**            | 6 DC or geared motors with PWM drivers (e.g., L298N)     |
| **Motor Driver**      | 2x L298N or equivalent                                   |
| **Ultrasonic Sensor** | HC-SR04 or similar                                       |
| **IMU Sensor**        | MPU6050 (via I2C)                                        |
| **Thermal Sensor**    | MLX90614 or analog sensor                                |
| **Power Monitor**     | Voltage divider or INA219                                |
| **Alarm**             | Buzzer (digital control)                                 |
| **Radio Module**      | NRF24L01, LoRa, or UART-based module                     |
| **LEDs**              | Debug/status LEDs                                        |
| **Button**            | For mode toggle / emergency stop                         |
| **Power Source**      | Li-ion battery pack with regulation                      |


---


## Future Additions

- Kalman filter for sensor fusion
- Onboard data logging to EEPROM/SD
- Autonomous obstacle avoidance using sensor mesh
- OTA command update or reflash support

---


# Implementation

## Section 01
break down and implement the first section of your Belfhym project, focusing on this part of the task diagram:

+----------------+
|  belfhym.c     |  <-- Entry Point
+--------+-------+
         |
         v
+------------------+       +------------------+       +------------------+
|   SensorTask     | --->  |  PathFinding     | --->  |   MotorTask      |
+------------------+       +------------------+       +------------------+



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
make
```

### License
This project is licensed under the GNU General Public License v3. See the LICENSE file for details.

---
