
#include "blfm_config.h"

#if BLFM_ENABLED_ESP32

#ifndef BLFM_ESP32_H
#define BLFM_ESP32_H

#include <stdint.h>
#include <stdbool.h>
#include "blfm_types.h"

// Initialize UART and buffer
void blfm_esp32_init(void);

// Call from UART RX interrupt for each received byte
void blfm_esp32_receive_byte(uint8_t byte);

// Application calls this to get parsed events
// Returns true if an event was available and filled
bool blfm_esp32_get_event(blfm_esp32_event_t *out_event);

#endif // BLFM_ESP32_H

#endif /* BLFM_ENABLED_ESP32 */
