#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

void log_init();

void log_print(const char *format, ...);

void log_memory(uint8_t *ptr, uint32_t size);