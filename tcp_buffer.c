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
  bytes_blocked_in_read = 0;
}

uint16_t *buffer_reserve() {
  if ((wb + 1) % depth == rb) {
    return NULL;
  }

  return buffer[wb];
}

uint16_t *buffer_consume() {
  if ((wb + 1) % depth == rb) {
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
void release_bytes(int bytec_count) {}
bool buffer_is_empty() { return wb == rb; }

bool buffer_is_full() { return (wb + 1) % depth == rb; }
int buffer_get_write_index() { return wb; }
int buffer_get_read_index() { return rb; }