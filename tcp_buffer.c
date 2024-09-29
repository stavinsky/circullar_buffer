#include "tcp_buffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint16_t buffer[depth][width] __attribute__((aligned(32)));
int wb = 0;
int rb = 0;
int in_use_index = 0;
int bytes_blocked_in_read = 0;

void buffer_reset() {
  wb = 0;
  rb = 0;
  in_use_index = 0;
  bytes_blocked_in_read = 0;
}

uint16_t *buffer_reserve_wr() {
  if ((wb + 1) % depth == in_use_index) {
    return NULL;
  }

  return buffer[wb];
}

uint16_t *buffer_done_wr() {
  if ((wb + 1) % depth == in_use_index) {
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
    bytes_blocked_in_read +=  width;
    return buf;
  }
  return NULL;
}
void buffer_release_no_blocked_update() { // TODO remove duplicate code
    if (in_use_index == rb) {
        return;
    }
    in_use_index = (in_use_index + 1) % depth;
}

void buffer_release() {
    if (in_use_index == rb) {
        return;
    }
    in_use_index = (in_use_index + 1) % depth;
    bytes_blocked_in_read -= width;
    if (bytes_blocked_in_read < 0) {
        bytes_blocked_in_read = 0;
    }
}
void buffer_release_bytes(int byte_count) {
    if (byte_count <= 0 || bytes_blocked_in_read <= 0) {
        bytes_blocked_in_read = 0; 
        return;
    }
    while(byte_count >= width) {
        byte_count -= width;
        buffer_release();
    }
    if ((bytes_blocked_in_read - byte_count) % width == 0 && bytes_blocked_in_read > 0) {
        bytes_blocked_in_read -= byte_count;
        buffer_release_no_blocked_update(); 
    }
    else {
        bytes_blocked_in_read -= byte_count;
    }

}

bool buffer_is_empty() { return wb == rb; }
bool buffer_is_full() { return (wb + 1) % depth == rb; }
int buffer_get_write_index() { return wb; }
int buffer_get_read_index() { return rb; }
int buffer_get_busy_index() { return in_use_index; }
int buffer_get_blocked_bytes() {return bytes_blocked_in_read; }