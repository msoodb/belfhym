
#include "blfm_esp32.h"
#include "libc_stubs.h"

static uint8_t rx_buffer[2];
static uint8_t rx_index = 0;

static volatile blfm_esp32_event_t pending_event;
static volatile bool event_ready = false;

void blfm_esp32_init(void) {
  rx_index = 0;
  event_ready = false;
  memset((void *)&pending_event, 0, sizeof(pending_event));
}

void blfm_esp32_receive_byte(uint8_t byte) {
  if (event_ready) {
    // Drop bytes until app takes event
    return;
  }

  rx_buffer[rx_index++] = byte;

  if (rx_index >= 2) {
    // We have a full frame
    pending_event.command = (blfm_esp32_command_type_t)rx_buffer[0];
    pending_event.speed = rx_buffer[1];
    // Optional timestamp (could use xTaskGetTickCount() if FreeRTOS used)
    pending_event.timestamp = 0;

    event_ready = true;
    rx_index = 0;
  }
}

bool blfm_esp32_get_event(blfm_esp32_event_t *out_event) {
  if (!event_ready) {
    return false;
  }

  if (out_event != NULL) {
    memcpy((void *)out_event, (const void *)&pending_event,
           sizeof(blfm_esp32_event_t));
  }

  event_ready = false;
  return true;
}
