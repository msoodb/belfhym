
# firmware
Your project (e.g. Belfhym) is not meant to run on your Linux system like a normal C program. 
It's firmware — code that runs directly on a microcontroller (e.g. STM32F103). 
There’s no operating system, standard C libraries, or system calls like on Linux.

# package
```sh
sudo dnf install arm-none-eabi-gcc arm-none-eabi-newlib openocd make git
```

# install
To use #include "stm32f1xx.h" without errors, 
you need to install the STM32 HAL/LL driver headers on your system and set up your project to find them. Here's how to do that:

# FreeRTOS
```sh
cd ~/Projects
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel
./freertos.sh
```

project structure for your STM32F103C8T6 FreeRTOS application in C. 
This structure will include all the necessary files and directories to get started with FreeRTOS on STM32.

belfhym/
│
├── Makefile               
├── README.md
├── src/              
│   ├── belfhym.c
│   ├── syscalls.c
│   ├── startup_stm32f103.c
│   ├── stm32f103_init.c 
│   ├── FreeRTOSConfig.h
│   └── peripherals.c
│
├── inc/                   # Header files
│   ├── belfhym.h
│   ├── syscalls.h
│   ├── FreeRTOSConfig.h
│   └── peripherals.h
│
├── startup/
│   └── startup_stm32f103.s
│
├── ld/ 
│   └── stm32f103.ld
│
├── CMSIS/                 # CMSIS headers for STM32F1
│   ├── core_cm3.h
│   └── device/            # Device-specific headers
│       ├── system_stm32f10x.c
│       ├── system_stm32f10x.h
│       ├── stm32f10x.h
│       └── stm32f103xb.h
│
└── FreeRTOS/              # FreeRTOS kernel source
    └── [FreeRTOS Kernel files...]

# Core Source Files
## Module	Purpose
belfhym.c/h	         Main entry point, init and loop logic
blfm_imu.c/h	     IMU communication and sensor fusion
blfm_pid.c/h	     PID controller implementation
blfm_motor.c/h	     Motor driver and mixing logic
blfm_radio.c/h	     BLE or RF communication interface
blfm_utils.c/h	     Math helpers, filters, timing
blfm_config.c/h	     Configuration, tuning constants
blfm_failsafe.c/h	 Failsafe and signal timeout handling
blfm_power.c/h	     Battery monitoring, power logic
blfm_debug.c/h	     Debug prints, LED blink, telemetry

## Optional / Future Modules
blfm_altitude.c/h	Altitude hold (barometer or sonar)
blfm_autopilot.c/h	Autonomous waypoint flight
blfm_storage.c/h	Persistent config via flash/EEPROM
blfm_ota.c/h	    Over-the-air firmware updates
blfm_sensors.c/h	Additional sensors (magnetometer, etc.)



           +-----------------+
           |   blfm_main     |
           +--------+--------+
                    |
     +--------------+--------------+
     |              |              |
+----v----+   +-----v----+   +-----v-----+
| blfm_imu|   | blfm_pid |   | blfm_radio|
+----+----+   +-----+----+   +-----+-----+
     |              |              |
     +-----+  +-----v----+   +-----v-----+
           |  | blfm_motor|   | blfm_failsafe |
           |  +-----+-----+   +--------------+
     +-----v----+   |
     | blfm_power|   |
     +-----+-----+   |
           |         |
     +-----v-----+   |
     | blfm_debug|<--+
     +-----------+

# Sources

## Core Module Responsibilities & How to Complete
1. blfm_main.c/h
   Role: Orchestrates everything: calls *_init() at startup and loops in *_update() functions.
   Start Here: You've already done this with belfhym.c/h.

2. blfm_imu.c/h
   Role: Communicates with the IMU over I2C, reads accelerometer/gyro, runs complementary or Kalman filter.
   Key functions:
	   blfm_imu_init()
	   blfm_imu_read()
	   blfm_imu_get_orientation()

3. blfm_pid.c/h
   Role: Accepts target and measured angles; computes PID output for each axis.
   Key functions:
	   blfm_pid_init()
	   blfm_pid_update(axis, target, measured)
	   blfm_pid_set_constants(...)

4. blfm_motor.c/h
   Role: Maps PID outputs to motor PWM; applies mixing logic (e.g., quad-X).
   Key functions:
	   blfm_motor_init()
	   blfm_motor_set_throttle(motor_id, pwm)
	   blfm_motor_update(pid_outputs)

5. blfm_radio.c/h
   Role: Handles incoming commands (via BLE or RF).
   Key functions:
	   blfm_radio_init()
	   blfm_radio_receive(command_struct*)

6. blfm_utils.c/h
   Role: Reusable math functions, filters, timing utilities.
   Functions:
	   blfm_map(), blfm_constrain(), blfm_filter_complementary(), etc.

7. blfm_config.c/h
   Role: Central place for constants: PID gains, IMU offsets, channel maps.
   Includes:
	   Defines for PID constants, radio channels, battery thresholds, etc.

8. blfm_failsafe.c/h
   Role: Watches for signal loss or bad inputs, kills motors if needed.
   Key functions:
	   blfm_failsafe_check()
	   blfm_failsafe_trigger()

9. blfm_power.c/h
   Role: Monitors battery voltage via ADC, triggers low-batt warning.
   Key functions:
	   blfm_power_read_voltage()
	   blfm_power_check_threshold()

10. blfm_debug.c/h
	Role: Debug print (via UART), LED blinking, log data.
	Key functions:
		blfm_debug_print()
		blfm_debug_led_toggle()
		blfm_debug_log_flight_data()

## Future / Optional Module Strategy
1. blfm_altitude: Use barometer/sonar to maintain height.
2. blfm_autopilot: Waypoint-based logic for autonomous flight.
3. blfm_storage: Store PID constants, config, IMU offsets.
4. blfm_ota: BLE-based firmware update (requires bootloader).
5. blfm_sensors: Integrate magnetometer or GPS for advanced navigation.

