#pragma once

#include <stdint.h>
#include <cstring>
extern void print_buf(const uint8_t *buf, size_t len);
extern uint32_t getFreeHeap(void);
extern uint32_t getTotalHeap(void);