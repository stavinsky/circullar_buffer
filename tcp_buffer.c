#include "tcp_buffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint16_t buffer[depth][width] __attribute__((aligned(32)));
int wb = 0;
int rb = 0;
int busy = 0;

int bytes_blocked_in_read = 0;
void buffer_reset() {
  wb = 0;
  rb = 0;
  busy = 0;
}

uint16_t *buffer_reserve_wr() {
  if ((wb + 1) % depth == busy) {
    return NULL;
  }

  return buffer[wb];
}

uint16_t *buffer_done_wr() {
  if ((wb + 1) % depth == busy) {
    return NULL;
  }

  uint16_t *buf = buffer[wb];
  wb = (wb + 1) % depth;

  return buf;
}

uint16_t *buffer_read() {
  if (rb != wb) {
    uint16_t *buf = buffer[rb];
    rb = (rb + 1) % depth;
    return buf;
  }
  return NULL;
}
void buffer_release() {
    if (busy == rb) {
        return;
    }
    busy = (busy + 1) % depth;
}

void release_bytes(int bytec_count) {}
bool buffer_is_empty() { return wb == rb; }
bool buffer_is_full() { return (wb + 1) % depth == rb; }
int buffer_get_write_index() { return wb; }
int buffer_get_read_index() { return rb; }
int buffer_get_busy_index() { return busy; }