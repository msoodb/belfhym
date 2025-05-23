
# Architectural Responsibilities

## Sensor Hub Task: 
	Periodically polls sensor data and sends it to the controller via a queue.
## Controller Task: 
	Receives sensor data, processes it, creates actuator commands, and sends them via another queue. 
## Actuator Hub Task: 
	Waits for and applies actuator commands.

---

# Belfhym Actuator Subsystem

## Overview

The actuator subsystem in the Belfhym project is designed to handle all output-related components, including:

* Motor control
* LCD display output
* LED status indication
* Alarm (e.g., buzzer)
* Radio transmission

This subsystem is modular, extensible, and real-time capable, leveraging FreeRTOS tasks and queues for asynchronous operation.

---

## Components

### 1. `blfm_actuator_hub`

Acts as the central manager for all actuators.

#### Functions:

* `blfm_actuator_hub_init()`: Initializes all actuators.
* `blfm_actuator_hub_start()`: Creates and starts one FreeRTOS task per actuator.
* `blfm_actuator_hub_send_command(cmd)`: Sends a composite command structure to individual actuator queues.

### 2. Command Structure

```c
typedef struct {
  blfm_motor_command_t motor;
  blfm_display_command_t display;
  blfm_led_state_t led;
  blfm_alarm_state_t alarm;
  blfm_radio_command_t radio;
} blfm_actuator_command_t;
```

Each field corresponds to one actuator module.

### 3. Task Architecture

Each actuator (motor, display, LED, alarm, radio) has:

* Its own queue (length 1, latest command always wins)
* Its own FreeRTOS task

#### General Task Loop Pattern:

```c
for (;;) {
  command_t cmd;
  if (xQueueReceive(queue, &cmd, portMAX_DELAY)) {
    apply_command(&cmd);
  }
}
```

### 4. Initialization and Application

Each module follows a consistent interface:

```c
void blfm_<module>_init(void);
void blfm_<module>_apply(const <module>_command_t *cmd);
```

---

## Example Flow

```c
// From controller:
blfm_actuator_command_t cmd = {
  .motor = { .speed = 128, .direction = 1 },
  .display = { .line1 = "GO", .line2 = "128 mm" },
  .led = LED_ON,
  .alarm = ALARM_OFF,
  .radio = { .message = "Status OK" }
};

blfm_actuator_hub_send_command(&cmd);
```

### Internally:

* Each command segment is copied to the respective actuator's queue.
* Tasks run independently and apply the commands.

---

## Benefits

* **Modular**: Easy to extend or modify individual actuators
* **Real-Time Safe**: Uses FreeRTOS queues for timing independence
* **Scalable**: Supports additional actuators without redesign
* **Decoupled**: Controller and actuators operate asynchronously

---

## Next Steps

Consider documenting:

* Sensor subsystem
* Controller logic
* Communication architecture between subsystems
