#ifndef S17_CONFIG_H
#define S17_CONFIG_H

#include "stm32f4xx.h"  /* IWYU pragma: keep */

// Nodes
#define S17_NODE_HERMES     0x42A7    /* Board A - Primary Controller */
#define S17_NODE_HOMA       0x8D1F    /* Board B - Secondary Controller */
#define S17_NODE_BELFHYM    0x3C95    /* Board C - Sensor Node */
#define S17_BROADCAST       0xFFFF    /* Broadcast to all devices */

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
#define S17_TASK_STACK_SIZE   512    /* Increased for encryption operations */
#define S17_SYSTEM_CLOCK_MHZ  84    /* STM32F411 typical system clock */
#define S17_USE_SOFTWARE_SPI  0     /* 0=Hardware SPI1, 1=Software SPI */

#endif // S17_CONFIG_H