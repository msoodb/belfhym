#ifndef S17_CONFIG_H
#define S17_CONFIG_H

#include "S17_roster.h"
#include "stm32f1xx.h"  /* For GPIO definitions */

/* Device Configuration - Each board can both transmit and receive */
/* Network addresses - keep these private for security */
#define S17_NETWORK_ADDRESS   {0xE7, 0xE7, 0xE7, 0xE7, 0xE8}    /* This device's network address */

/* Device Identity - CHANGE THIS for each board */
#define S17_THIS_DEVICE_ID    S17_NODE_BELFHYM    /* This board's callsign */


/* Security Configuration - CHANGE THIS for each board! */
#define S17_ENCRYPTION_KEY {0x4A, 0x75, 0x2E, 0x93, 0xC1, 0x58, 0x7B, 0xA4, \
                           0xD6, 0x19, 0x8C, 0x3F, 0xE2, 0x6B, 0x94, 0x17}
/* IMPORTANT: Use same key on all boards that need to communicate */
/* Generate random key at: https://www.random.org/bytes/ */

/* Hardware Pins (STM32F1 + nRF24L01) - CORRECT pins from working hermes */
#define S17_NRF24_PORT        GPIOA
#define S17_NRF24_CE_PIN      4     /* PA4 */
#define S17_NRF24_CSN_PIN     0     /* PA0 - CRITICAL: This was wrong! */
#define S17_NRF24_SCK_PIN     5     /* PA5 - SPI1_SCK */
#define S17_NRF24_MOSI_PIN    7     /* PA7 - SPI1_MOSI */
#define S17_NRF24_MISO_PIN    6     /* PA6 - SPI1_MISO */
#define S17_NRF24_IRQ_PIN     1     /* PA1 */

/* RF Settings - matching working configuration */
#define S17_RF_CHANNEL        76    /* 2.476 GHz */
#define S17_RF_POWER_LEVEL    3     /* 0dBm */
#define S17_RF_DATA_RATE      0     /* 1Mbps */

/* System Settings */
#define S17_MAX_PAYLOAD_SIZE  32    /* Must match between TX and RX */
#define S17_TX_BUFFER_SIZE    8
#define S17_RX_BUFFER_SIZE    8
#define S17_UPDATE_RATE_MS    50
#define S17_STACK_SIZE        384     /* Sufficient for encryption with optimized buffers */
#define S17_SYSTEM_CLOCK_MHZ  72
#define S17_USE_SOFTWARE_SPI  0     /* Use hardware SPI1 like working code */

#endif // S17_CONFIG_H

