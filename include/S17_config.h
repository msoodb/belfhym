/**
 * S17 Configuration for Belfhym Tactical Node
 * Operation Iron Fist - Battle Rover
 * 
 * Device: Belfhym (Battle Rover)
 * Role: Tactical Node
 * Mission: Operation Iron Fist
 */

#ifndef S17_CONFIG_H
#define S17_CONFIG_H

#include <stdint.h>
#include "stm32f1xx.h"

/* ========================================================================== */
/* NETWORK ROSTER - Device IDs for this tactical network                     */
/* ========================================================================== */

/* Network Operations Controllers (NOC) - Range 0x1000-0x1FFF */
#define S17_NOC_HERMES            0x1001    /* Hermes NOC - Primary controller */
#define S17_NOC_BACKUP            0x1002    /* Backup NOC (if needed) */

/* Tactical Nodes - Range 0x2000-0x2FFF */  
#define S17_TN_HOMA               0x2001    /* Homa - RC Plane */
#define S17_TN_BELFHYM            0x2002    /* Belfhym - Battle Rover */
#define S17_TN_DELTA              0x2003    /* Delta - Additional Node */
#define S17_TN_ECHO               0x2004    /* Echo - Additional Node */
#define S17_TN_FOXTROT            0x2005    /* Foxtrot - Additional Node */
#define S17_TN_GOLF               0x2006    /* Golf - Additional Node */
#define S17_TN_HOTEL              0x2007    /* Hotel - Additional Node */

/* Device Types */
#define S17_TYPE_NOC              1         /* Net Operations Controller */
#define S17_TYPE_TN               2         /* Tactical Node */

/* Special Network IDs */
#define S17_BROADCAST             0xFFFF    /* Broadcast to all devices */

/* ========================================================================== */
/* FACTORY/DEVICE CONFIGURATION - Set once per device                        */
/* ========================================================================== */

/* Device Identity - BELFHYM TACTICAL NODE CONFIGURATION */
#define S17_DEVICE_ID             S17_TN_BELFHYM    /* Belfhym Tactical Node ID */
#define S17_DEVICE_TYPE           S17_TYPE_TN       /* Tactical Node */
#define S17_DEVICE_SERIAL         "BELFHYM-001"     /* Belfhym serial number */

/* Root Master Key (RMK) - UNIQUE per device, permanent */
#define S17_DEVICE_RMK { \
    0xC3, 0x4D, 0x5E, 0x6F, 0x70, 0x81, 0x92, 0xA3, \
    0xB4, 0xC5, 0xD6, 0xE7, 0xF8, 0x09, 0x1A, 0x2B, \
    0x3C, 0x4D, 0x5E, 0x6F, 0x70, 0x81, 0x92, 0xA3, \
    0xB4, 0xC5, 0xD6, 0xE7, 0xF8, 0x09, 0x1A, 0x2C  \
}

/* Hardware Configuration - STM32F103 + NRF24L01+ */
#define S17_NRF24_CE_PIN          4     /* PA4 - Chip Enable */
#define S17_NRF24_CSN_PIN         0     /* PA0 - SPI Chip Select */
#define S17_NRF24_SCK_PIN         5     /* PA5 - SPI1_SCK */
#define S17_NRF24_MOSI_PIN        7     /* PA7 - SPI1_MOSI */
#define S17_NRF24_MISO_PIN        6     /* PA6 - SPI1_MISO */
#define S17_NRF24_IRQ_PIN         1     /* PA1 - IRQ (optional interrupt) */

/* Hardware Capabilities */
#define S17_HW_VERSION            0x0103        /* Hardware revision */
#define S17_FW_VERSION            0x0200        /* Firmware version */
#define S17_RF_POWER_CLASS        3             /* RF power class (0-3) */

/* ========================================================================== */
/* MISSION/OPERATIONAL CONFIGURATION - Change per mission                    */
/* ========================================================================== */

/* Mission Identity */
#define S17_MISSION_ID            "OP-IRON-FIST"   /* Mission identifier */
#define S17_MISSION_CALLSIGN      "BELFHYM-2"      /* Radio callsign */
#define S17_MISSION_NET_ID        0x49524F4E       /* "IRON" network ID */

/* Network/Mission Key (NMK) - Generate fresh per mission */
#define S17_MISSION_NMK { \
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, \
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C, \
    0x76, 0x2E, 0x7C, 0x4C, 0x63, 0x7B, 0x87, 0x2D, \
    0xDB, 0x4F, 0x81, 0xA5, 0x9C, 0xE8, 0x37, 0xF2  \
}

/* Network Topology - Expected nodes for this mission */
#define S17_MISSION_EXPECTED_NODES    3            /* Hermes + Homa + Belfhym */
#define S17_MISSION_NODE_HOMA_SLOT    1            /* Homa's TDMA slot */
#define S17_MISSION_NODE_BELFHYM_SLOT 2            /* Belfhym's TDMA slot */

/* Frequency Plan */
#define S17_RF_CHANNEL            76           /* Primary channel (2.476 GHz) */
#define S17_HOP_CHANNELS          {76, 77, 78, 79, 80, 81, 82, 83, \
                                   84, 85, 86, 87, 88, 89, 90, 91}  /* Hop sequence */
#define S17_HOP_INTERVAL_MS       250          /* Channel hop every 250ms */
#define S17_EMERGENCY_CHANNEL     125          /* Emergency backup channel */

/* ========================================================================== */
/* PROTOCOL CONFIGURATION - Usually don't change                             */
/* ========================================================================== */

/* Payload Sizes */
#define S17_MAX_PAYLOAD_SIZE      32           /* NRF24L01+ maximum */
#define S17_MAX_APP_PAYLOAD       10           /* Application data per packet */
#define S17_ENCRYPTED_OVERHEAD    22           /* Counter(2) + Epoch(4) + GCM_tag(16) */

/* Network Limits */
#define S17_MAX_DEVICES           8            /* Max devices in network */
#define S17_MAX_TRACKED_DEVICES   8            /* Device tracking limit */

/* TDMA Timing */
#define S17_SLOT_DURATION_MS      25           /* Each TDMA slot duration */
#define S17_SLOTS_PER_FRAME       8            /* Total slots per frame */
#define S17_FRAME_DURATION_MS     (S17_SLOT_DURATION_MS * S17_SLOTS_PER_FRAME)

/* Network Timing */
#define S17_BEACON_INTERVAL_MS    1000         /* NOC beacon every 1 second */
#define S17_HEARTBEAT_INTERVAL_MS 5000         /* Node heartbeat interval */
#define S17_OFFLINE_TIMEOUT_MS    15000        /* Node offline detection */

/* Security Parameters */
#define S17_NETWORK_KEY_SIZE      32           /* 256-bit keys */
#define S17_GCM_TAG_SIZE         16           /* AES-GCM auth tag size */
#define S17_TEK_ROTATION_MIN     15           /* TEK rotation every 15 min */
#define S17_JOIN_TIMEOUT_SEC     30           /* Node join timeout */
#define S17_MAX_REPLAY_WINDOW    1000         /* Anti-replay window */

/* Memory Optimization */
#define S17_TASK_STACK_SIZE      256          /* FreeRTOS stack (words) */

/* ========================================================================== */
/* HARDWARE CONFIGURATION                                                    */
/* ========================================================================== */

/* Hardware Simulation (for STM32F103 without ATECC608B) */
#define S17_SIMULATE_SECURE_ELEMENT 1          /* Simulate ATECC608B in software */

/* ========================================================================== */
/* VALIDATION AND MAGIC NUMBERS                                              */
/* ========================================================================== */

/* Configuration validation */
#define S17_CONFIG_MAGIC          0x53313743   /* "S17C" - Config loaded */
#define S17_CONFIG_VERSION        0x0200       /* Config format version */

/* ========================================================================== */
/* CONVENIENCE MACROS                                                        */
/* ========================================================================== */

/* Device role detection - Belfhym is always TN */
#define S17_IS_NOC()              (false)  /* Belfhym is never NOC */
#define S17_IS_TACTICAL_NODE()    (true)   /* Belfhym is always TN */
#define S17_DEVICE_ROLE           S17_TIMING_SLAVE   /* Always slave */

/* Network address calculation */
#define S17_NETWORK_ADDRESS       {0xE7, 0xE7, 0xE7, 0xE7, (S17_MISSION_NET_ID & 0xFF)}

/* Key generation helpers */
#define S17_DEVICE_RMK_ARRAY      static const uint8_t device_rmk[32] = S17_DEVICE_RMK
#define S17_MISSION_NMK_ARRAY     static const uint8_t mission_nmk[32] = S17_MISSION_NMK
#define S17_HOP_CHANNELS_ARRAY    static const uint8_t hop_channels[] = S17_HOP_CHANNELS

#endif /* S17_CONFIG_H */