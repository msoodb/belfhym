
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_TYPES_H
#define BLFM_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "S17_config.h"

//==============================================================================
// OLED DISPLAY CONSTANTS
//==============================================================================

#define BLFM_OLED_WIDTH 128
#define BLFM_OLED_HEIGHT 32
#define BLFM_OLED_PAGES (BLFM_OLED_HEIGHT / 8)
#define BLFM_OLED_MAX_SMALL_TEXT_LEN 12
#define BLFM_OLED_MAX_BIG_TEXT_LEN 16


//==============================================================================
// SENSORS
//==============================================================================

// Event definitions

typedef struct {
  uint8_t data[S17_MAX_PAYLOAD_SIZE];
  uint8_t length;
  uint8_t pipe;
  int8_t rssi;
} blfm_nrf24_event_t;

typedef struct {
  uint16_t distance_mm;
} blfm_ultrasonic_data_t;

/* IR remote functionality removed from Belfhym */

typedef struct {
  int16_t x_normalized;   /* X value (-1000 to +1000) from hermes joystick */
  int16_t y_normalized;   /* Y value (-1000 to +1000) from hermes joystick */
  bool button_pressed;    /* Button state from hermes joystick */
  uint32_t timestamp;     /* Received timestamp */
} blfm_joystick_event_t;

typedef struct {
  int16_t x_normalized;   /* X value (-1000 to +1000) from single joystick */
  int16_t y_normalized;   /* Y value (-1000 to +1000) from single joystick */
  bool button_pressed;    /* Button state from single joystick */
} blfm_single_joystick_t;

typedef struct {
  blfm_single_joystick_t left;    /* Left joystick data (for motors) */
  blfm_single_joystick_t right;   /* Right joystick data (for servos) */
  uint32_t timestamp;             /* Received timestamp */
} blfm_dual_joystick_event_t;


/* RC Plane Sensor Data Structures */
typedef struct {
    int16_t gyro_x;         /* Gyroscope X-axis (deg/s * 100) */
    int16_t gyro_y;         /* Gyroscope Y-axis (deg/s * 100) */
    int16_t gyro_z;         /* Gyroscope Z-axis (deg/s * 100) */
    int16_t accel_x;        /* Accelerometer X-axis (mg) */
    int16_t accel_y;        /* Accelerometer Y-axis (mg) */
    int16_t accel_z;        /* Accelerometer Z-axis (mg) */
    int16_t temp_c;         /* Temperature (°C * 10) */
    bool valid;             /* Data validity flag */
    uint32_t timestamp_ms;  /* Measurement timestamp */
} blfm_imu_data_t;

typedef struct {
    int16_t mag_x;          /* Magnetometer X-axis (mGauss) */
    int16_t mag_y;          /* Magnetometer Y-axis (mGauss) */
    int16_t mag_z;          /* Magnetometer Z-axis (mGauss) */
    uint16_t heading_deg;   /* Calculated heading (0-359°) */
    bool valid;             /* Data validity flag */
    uint32_t timestamp_ms;  /* Measurement timestamp */
} blfm_magnetometer_data_t;

typedef struct {
    int32_t pressure_pa;    /* Pressure in Pascals */
    int32_t altitude_cm;    /* Altitude in centimeters above sea level */
    int16_t temperature_c10; /* Temperature (°C * 10) */
    bool valid;             /* Data validity flag */
    uint32_t timestamp_ms;  /* Measurement timestamp */
} blfm_barometer_data_t;

typedef struct {
    int32_t latitude;       /* Latitude (degrees * 1e7) */
    int32_t longitude;      /* Longitude (degrees * 1e7) */
    int32_t altitude_mm;    /* GPS altitude in millimeters */
    uint16_t speed_cmps;    /* Ground speed (cm/s) */
    uint16_t course_deg;    /* Course over ground (0-359°) */
    uint8_t satellites;     /* Number of satellites */
    uint8_t fix_quality;    /* GPS fix quality (0=invalid, 1=GPS, 2=DGPS) */
    bool valid;             /* Data validity flag */
    uint32_t timestamp_ms;  /* Measurement timestamp */
} blfm_gps_data_t;

typedef struct {
    uint16_t airspeed_cmps; /* Airspeed in cm/s */
    int16_t static_pressure; /* Static pressure differential */
    bool valid;             /* Data validity flag */
    uint32_t timestamp_ms;  /* Measurement timestamp */
} blfm_airspeed_data_t;

typedef struct {
    uint16_t voltage_mv;    /* Battery voltage in millivolts */
    int16_t current_ma;     /* Battery current in milliamps (+ = discharge) */
    uint16_t remaining_pct; /* Estimated remaining capacity (0-100%) */
    uint32_t consumed_mah;  /* Total consumed capacity in mAh */
    bool valid;             /* Data validity flag */
    uint32_t timestamp_ms;  /* Measurement timestamp */
} blfm_battery_data_t;

/* Sensor fusion and flight state data */
typedef struct {
    /* Attitude (Euler angles in degrees * 100) */
    int16_t roll_deg100;    /* Roll angle (-18000 to +18000) */
    int16_t pitch_deg100;   /* Pitch angle (-9000 to +9000) */
    int16_t yaw_deg100;     /* Yaw angle (0 to 35999) */
    
    /* Angular rates (degrees/second * 100) */
    int16_t roll_rate_dps100;
    int16_t pitch_rate_dps100;
    int16_t yaw_rate_dps100;
    
    /* Linear accelerations (m/s² * 100) */
    int16_t accel_x_ms2_100;
    int16_t accel_y_ms2_100;
    int16_t accel_z_ms2_100;
    
    bool attitude_valid;    /* Attitude estimation validity */
    uint32_t timestamp_ms;  /* Fusion timestamp */
} blfm_attitude_data_t;

/* Complete sensor data structure */
typedef struct {
    /* Raw sensor data */
    blfm_ultrasonic_data_t ultrasonic;
    blfm_imu_data_t imu;
    blfm_magnetometer_data_t magnetometer;
    blfm_barometer_data_t barometer;
    blfm_gps_data_t gps;
    blfm_airspeed_data_t airspeed;
    blfm_battery_data_t battery;
    
    /* Processed/fused data */
    blfm_attitude_data_t attitude;
    
    /* System health */
    uint8_t sensor_health_flags; /* Bit field for sensor health status */
    uint32_t system_timestamp_ms; /* Overall system timestamp */
} blfm_sensor_data_t;


//==============================================================================
// ACTUATORS
//==============================================================================

typedef struct {
  uint16_t speed;     // 0–255
  uint8_t direction; // 0 = forward, 1 = backward
} blfm_single_motor_command_t;

typedef struct {
  blfm_single_motor_command_t left;
  blfm_single_motor_command_t right;
} blfm_motor_command_t;


typedef enum {
  BLFM_SERVO_TYPE_SCANNER = 0,     // Continuous scanning motion
  BLFM_SERVO_TYPE_TRACKER,         // Position tracking based on sensors
  BLFM_SERVO_TYPE_MANUAL,          // Manual control only
  BLFM_SERVO_TYPE_STATIC,          // Fixed position
  BLFM_SERVO_TYPE_PROPORTIONAL,    // Precise proportional positioning
  BLFM_SERVO_TYPE_RADAR            // Radar sweep from extreme left to right
} blfm_servo_type_t;

typedef struct {
  uint8_t angle;                    // Target angle (0-180)
  uint16_t pulse_width_us;          // Optional: direct pulse width (1000-2000us)
  
  // Scanner-specific parameters
  uint8_t scan_min_angle;           // Scanner minimum angle (default: 0)
  uint8_t scan_max_angle;           // Scanner maximum angle (default: 180)
  uint8_t scan_step;                // Scanner step size (default: 5)
  uint16_t scan_delay_ms;           // Scanner delay between steps (default: 100)
  
  // Tracker-specific parameters
  int16_t target_x;                 // Target X coordinate for tracking
  int16_t target_y;                 // Target Y coordinate for tracking
  uint8_t tracking_speed;           // Tracking movement speed (1-10)
  
  // Proportional-specific parameters
  int16_t proportional_input;       // Input value (-1000 to +1000)
  uint8_t deadband;                 // Deadband around center (0-50)
  uint8_t travel_limit;             // Travel limit percentage (50-100)
  
  // General parameters
  uint8_t speed;                    // Movement speed (1-10, 10=fastest)
  bool enable_smooth;               // Enable smooth movement
  bool reverse_direction;           // Reverse servo direction
} blfm_servomotor_command_t;


typedef enum {
  BLFM_OLED_ICON_NONE = 0,
  BLFM_OLED_ICON_HEART,
  BLFM_OLED_ICON_SMILEY,
  BLFM_OLED_ICON_STAR,
} blfm_oled_icon_t;

typedef struct {
  blfm_oled_icon_t icon1;  // top-left icon
  blfm_oled_icon_t icon2;  // top-left icon (next to icon1)
  blfm_oled_icon_t icon3;  // top-right icon
  blfm_oled_icon_t icon4;  // top-right icon (next to icon3)

  char smalltext1[BLFM_OLED_MAX_SMALL_TEXT_LEN];
  char bigtext[BLFM_OLED_MAX_BIG_TEXT_LEN];
  char smalltext2[BLFM_OLED_MAX_SMALL_TEXT_LEN];

  uint8_t invert;          // invert display
  uint8_t progress_percent; // optional progress bar

} blfm_oled_command_t;


typedef enum {
  BLFM_LED_MODE_OFF = 0,
  BLFM_LED_MODE_ON,
  BLFM_LED_MODE_BLINK,
} blfm_led_mode_t;

typedef struct {
  blfm_led_mode_t mode;    // off, on, or blink
  uint16_t on_time;        // LED on duration in milliseconds
  uint16_t off_time;       // LED off duration in milliseconds
} blfm_led_command_t;

typedef struct {
  bool should_broadcast;   // Whether to send NRF24 broadcast
  uint16_t dest_id;       // Destination device ID (from S17_roster.h)
  uint8_t data[S17_MAX_PAYLOAD_SIZE];       // Broadcast data payload
  uint8_t length;         // Data length (max S17_MAX_PAYLOAD_SIZE bytes)
} blfm_nrf24_command_t;

/* Unified controller input event */
typedef enum {
  BLFM_INPUT_SENSOR,      // Sensor data update
  BLFM_INPUT_NRF24,       // NRF24 received data
  BLFM_INPUT_JOYSTICK,    // Single joystick data from hermes (legacy)
  BLFM_INPUT_DUAL_JOYSTICK // Dual joystick data from hermes (motors + servos)
} blfm_input_type_t;

typedef struct {
  blfm_input_type_t type;
  union {
    blfm_sensor_data_t sensor;
    blfm_nrf24_event_t nrf24;
    blfm_joystick_event_t joystick;
    blfm_dual_joystick_event_t dual_joystick;
  } data;
} blfm_controller_input_t;

typedef struct {
  blfm_motor_command_t motor;
  blfm_oled_command_t oled;
  blfm_led_command_t led;
  blfm_servomotor_command_t servo1;
  blfm_servomotor_command_t servo2;
  blfm_servomotor_command_t servo3;
  blfm_servomotor_command_t servo4;
  blfm_nrf24_command_t nrf24;
} blfm_controller_output_t;

#endif /* BLFM_TYPES_H */
