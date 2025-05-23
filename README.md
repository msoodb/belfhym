# Belfhym: FreeRTOS-Based Lunar Rover Robot

**Belfhym** is a modular, real-time lunar rover robot powered by **FreeRTOS** and **CMSIS**, designed for:

- Autonomous terrain navigation  
- Multi-sensor fusion  
- Real-time telemetry  
- Manual override via remote command  

  
Built for the **STM32F103C8T6 (Blue Pill)**, Belfhym integrates multiple control and sensing subsystems into a clean, multitasking environment with well-defined inter-task communication and scheduling policies.

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

This table outlines the tasks in the system, their responsibilities, priorities, and communication mechanisms.

| Task Name         | Responsibility                               | Priority | Communication                    |
|------------------|----------------------------------------------|----------|----------------------------------|
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



3. Conceptual Communication Flow
```
[ Sensors ] → [ Sensor Hub ] → [ Controller ] → [ Actuator Hub ] → [ Actuators ]
```
- Sensors: Collect raw data from environment (IMU, Ultrasonic, etc.)
- Sensor Hub: Polls sensors, aggregates and timestamps data
- Controller: Performs logic decisions (PID, pathfinding, safety checks)
- Actuator Hub: Receives commands and routes them to the appropriate actuator
- Actuators: Motor, LED, Alarm, Display, etc.

4. Data Flow and Task Design (FreeRTOS)
   1. Tasks
	  - SensorTask: Reads raw sensor data, sends it to SensorDataQueue.
	  - ControllerTask: Receives from SensorDataQueue, processes data, sends output to ControlCommandQueue.
	  - ActuatorTask: Reads from ControlCommandQueue, executes actuator commands.
	2. Queues
	  - SensorDataQueue: Transmits blfm_sensor_data_t (struct with all sensor data).
	  - ControlCommandQueue: Transmits blfm_actuator_command_t (per-actuator commands).

```
+---------------------+
|    Main Entry       |   (belfhym.c)
+----------+----------+
           |
           v
+---------------------+
|     System Init     |  (board init, clocks, RTOS)
+----------+----------+
           |
           v
+=====================+
||   Task Manager    ||  ← starts all system tasks
+=====================+
           |
           v
+--------------------------------------+
|         System Services Layer        |
+-------------------+------------------+
| Sensor Hub        | Actuator Hub     |
| (all sensors)     | (all motors, LED)|
+-------------------+------------------+
| Communication     | Logging          |
| (Manual + Remote) | (Telemetry/Debug)|
+-------------------+------------------+
| Safety Monitor    | Power Manager    |
+-------------------+------------------+

Tasks in Services Layer exchange data via FreeRTOS queues/events/mutexes
           |
           v
+---------------------+
| Application Logic   | ← Pathfinding, PID, AI, etc.
+---------------------+

├── actuators
├── app
├── communication
├── control
├── core
├── drivers
├── interface
├── logging
├── logic
├── monitoring
├── power
├── safety
└── sensors
```

```bash
bear -- make
script/structure.sh
cat modules.txt | script/modules.sh
```


## WARNING
FreeRTOS/portable/GCC/ARM_CM3/port.c
```code 
const portISR_t * const pxVectorTable = portSCB_VTOR_REG;
(void)pxVectorTable;  // Suppress unused variable warning added by me.
```

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


## Getting Started


### Prerequisites toolchain for Development Environment (Fedora Linux)

```bash
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib
arm-none-eabi-gcc --version
```

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

### License
This project is licensed under the GNU General Public License v3. See the LICENSE file for details.

---
