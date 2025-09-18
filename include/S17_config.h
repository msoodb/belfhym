#ifndef S17_CONFIG_H
#define S17_CONFIG_H

#include "stm32f4xx.h"  /* IWYU pragma: keep */

/* ========================================================================== */
/* NETWORK ROSTER - Device IDs for this tactical network                     */
/* ========================================================================== */

/* Network Operations Controllers (NOC) - Range 0x1000-0x1FFF */
#define S17_NOC_HERMES            0x1001    /* Hermes NOC - Primary controller */
#define S17_NOC_BACKUP            0x1002    /* Backup NOC (if needed) */

/* Tactical Nodes - Range 0x2000-0x2FFF */
#define S17_TN_HOMA               0x2001    /* Homa - RC Plane */
#define S17_TN_BELFHYM            0x2002    /* Belfhym - Battle Rover */

/* Device Types */
#define S17_TYPE_NOC              1         /* Net Operations Controller */
#define S17_TYPE_TN               2         /* Tactical Node */

/* Special Network IDs */
#define S17_BROADCAST             0xFFFF    /* Broadcast to all devices */

/* Legacy compatibility (can be removed later) */
#define S17_NODE_HERMES     S17_NOC_HERMES    /* Board A - Primary Controller */
#define S17_NODE_HOMA       S17_TN_HOMA       /* Board B - Secondary Controller */
#define S17_NODE_BELFHYM    S17_TN_BELFHYM    /* Board C - Sensor Node */

/* Device Configuration - Each board can both transmit and receive */
#define S17_NETWORK_ADDRESS   {0xE7, 0xE7, 0xE7, 0xE7, 0xE8}
#define S17_THIS_DEVICE_ID    S17_NODE_BELFHYM

/* Hardware Pins (STM32F411 + nRF24L01) */
#define S17_NRF24_PORT        GPIOA
#define S17_NRF24_CE_PIN      4     /* PA4 */
#define S17_NRF24_CSN_PIN     0     /* PA0 */
#define S17_NRF24_SCK_PIN     5     /* PA5 - SPI1_SCK */
#define S17_NRF24_MOSI_PIN    7     /* PA7 - SPI1_MOSI */
#define S17_NRF24_MISO_PIN    6     /* PA6 - SPI1_MISO */
#define S17_NRF24_IRQ_PIN     1     /* PA1 */

/* RF Settings */
#define S17_RF_CHANNEL        76    /* 2.476 GHz - CHANGE IF INTERFERENCE */
#define S17_RF_POWER_LEVEL    3     /* 0dBm (0=Min, 3=Max) */
#define S17_RF_DATA_RATE      0     /* 1Mbps (0=1Mbps, 1=2Mbps) */

/* System Settings */
#define S17_MAX_PAYLOAD_SIZE  32    /* Must match between TX and RX */
#define S17_TX_BUFFER_SIZE    8
#define S17_RX_BUFFER_SIZE    8
#define S17_UPDATE_RATE_MS    50
#define S17_STACK_SIZE   512    /* Increased for encryption operations */
#define S17_SYSTEM_CLOCK_MHZ  84    /* STM32F411 typical system clock */
#define S17_USE_SOFTWARE_SPI  0     /* 0=Hardware SPI1, 1=Software SPI */

/* ========================================================================== */
/* DEVICE CONFIGURATION - BELFHYM TACTICAL NODE                              */
/* ========================================================================== */

/* Device Identity - BELFHYM as Tactical Node (Battle Rover) */
#define S17_DEVICE_ID             S17_TN_BELFHYM   /* Belfhym TN ID */
#define S17_DEVICE_TYPE           S17_TYPE_TN      /* Tactical Node */
#define S17_DEVICE_SERIAL         "BELFHYM-001"    /* Human readable serial */

/* ========================================================================== */
/* SECURITY CONFIGURATION - Military-grade features                          */
/* ========================================================================== */

/* Security Enable/Disable */
#define S17_SECURITY_ENABLED      0           /* Start DISABLED for safe migration */
#define S17_NETWORK_KEY_SIZE      32          /* 256-bit keys */
#define S17_GCM_TAG_SIZE         16          /* AES-GCM auth tag size */
#define S17_TEK_ROTATION_MIN     15          /* TEK rotation every 15 min */
#define S17_MAX_REPLAY_WINDOW    1000        /* Anti-replay window */

/* Root Master Key (RMK) - UNIQUE per device, permanent */
#define S17_DEVICE_RMK { \
    0xC3, 0x4D, 0x5E, 0x6F, 0x70, 0x81, 0x92, 0xA3, \
    0xB4, 0xC5, 0xD6, 0xE7, 0xF8, 0x09, 0x1A, 0x2B, \
    0x3C, 0x4D, 0x5E, 0x6F, 0x70, 0x81, 0x92, 0xA3, \
    0xB4, 0xC5, 0xD6, 0xE7, 0xF8, 0x09, 0x1A, 0x2C  \
}

/* Network/Mission Key (NMK) - Same per mission across all devices */
#define S17_MISSION_NMK { \
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, \
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C, \
    0x76, 0x2E, 0x7C, 0x4C, 0x63, 0x7B, 0x87, 0x2D, \
    0xDB, 0x4F, 0x81, 0xA5, 0x9C, 0xE8, 0x37, 0xF2  \
}

/* Frequency Hopping Configuration */
#define S17_HOP_CHANNELS          {76, 77, 78, 79, 80, 81, 82, 83, \
                                   84, 85, 86, 87, 88, 89, 90, 91}  /* Hop sequence */
#define S17_HOP_INTERVAL_MS       250          /* Channel hop every 250ms */

/* TDMA Configuration */
#define S17_SLOT_DURATION_MS      25           /* Each TDMA slot duration */
#define S17_SLOTS_PER_FRAME       8            /* Total slots per frame */
#define S17_MISSION_NODE_HOMA_SLOT    1        /* Homa's TDMA slot */
#define S17_MISSION_NODE_BELFHYM_SLOT 2        /* Belfhym's TDMA slot */

/* ========================================================================== */
/* CONVENIENCE MACROS                                                        */
/* ========================================================================== */

/* Device role detection */
#define S17_IS_NOC()              (S17_DEVICE_TYPE == S17_TYPE_NOC)
#define S17_IS_TACTICAL_NODE()    (S17_DEVICE_TYPE == S17_TYPE_TN)

#endif // S17_CONFIG_H
