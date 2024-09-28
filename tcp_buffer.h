#ifndef TCP_BUFFER_H
#define TCP_BUFFER_H
#include <stdbool.h>
#include <stdint.h>
#define depth 10
#define width 20000
void buffer_reset(void);
uint16_t *buffer_consume(void);
uint16_t *buffer_read(void);
bool buffer_is_empty(void);
bool buffer_is_full(void);
int buffer_get_write_index(void);
int buffer_get_read_index(void);
uint16_t *buffer_reserve(void);
#endif