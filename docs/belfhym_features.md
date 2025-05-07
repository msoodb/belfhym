
# Fly size

```
	Scale	          Wing/Prop Span	Typical Weight	Feasible For You?
	Insect-size	      1–3 cm	        <1g	             Not yet — too complex
	Palm-size	      10–15 cm	        20–40g	         Ideal for first working prototype
	Larger drone	  20–30 cm	        100–300g	     Easier to test, but less portable/micro-like
```


# Project Checklist
Starting with a palm-sized drone is a smart, achievable path — you'll learn everything from motor control and PID to wireless communication and embedded C.
Here’s a step-by-step checklist, tasklist, and a clean C project structure to get your prototype in the air.

## Project Requirements

### Hardware

- Flight frame (3D printed or carbon fiber)
- 4× brushed/brushless motors (coreless 6mm or 7mm brushed motors are great)
- 4× propellers (CW/CCW matched)
- LiPo battery (1S 3.7V 150–300 mAh)
- Motor driver or ESCs (depends on motor type)
- Microcontroller (STM32F0/F1/L4, RP2040, or nRF52)
- IMU (e.g., MPU6050 or ICM20948)
- Wireless module (BLE, RF24, or ESP8266)
- Power regulation (boost or LDO for 3.3V)
- USB UART or SWD programmer

### Tools

- KiCad (for PCB design)
- STM32CubeIDE or PlatformIO (for embedded C development)
- Oscilloscope or logic analyzer (optional)
- 3D printer or laser cut frame
- Soldering tools and gear

---

## Task List by Phases

### Phase 1: Ground Control (non-flying)

- Blink LED and basic MCU setup
- Initialize IMU and verify raw readings
- Set up motor drivers (PWM or GPIO)
- Connect wireless module and receive simple commands
- Write motor test command (e.g., `MOTOR1 30%`)

### Phase 2: Stabilization

- Implement sensor fusion (Complementary or Kalman filter)
- Write PID controller for pitch, roll, and yaw
- Tune PID on ground (simulate tilt corrections)
- Write safety features (e.g., low battery cutoff)

### Phase 3: First Flight

- Map wireless control to throttle, yaw, pitch, and roll
- Gradually test hover and stability
- Tune PID in-flight
- Record telemetry via UART or BLE

### Phase 4: Security and Refinement

- Add checksum or encryption to wireless commands
- Add failsafe (loss of signal → cut throttle)
- Calibrate IMU offsets
- OTA updates (optional)
- Optimize power consumption

# Project
belphym/
├── include/
│   ├── imu.h
│   ├── motor.h
│   ├── pid.h
│   ├── radio.h
│   ├── utils.h
├── src/
│   ├── main.c              // Init loop
│   ├── imu.c               // IMU init and sensor fusion
│   ├── motor.c             // PWM or ESC control
│   ├── pid.c               // PID controller
│   ├── radio.c             // RF/BLE input parsing
│   ├── utils.c             // math helpers, battery check, etc
├── lib/                    // Optional: third-party drivers (e.g., MPU6050)
├── firmware/               // Precompiled binaries
├── board/                  // Board-specific pin definitions
│   ├── board.h
├── Makefile or platformio.ini
├── README.md


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

