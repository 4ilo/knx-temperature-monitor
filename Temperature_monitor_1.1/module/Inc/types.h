#ifndef TYPES
#define TYPES

#include <stdint.h>

typedef struct {
    float outside_temp;
    float water_temp;
    uint32_t lux;
    uint8_t power_230v;
    uint8_t power_24v;
    uint8_t timer;
}input_values_t;

typedef struct {
    uint8_t alarm_230v;
    uint8_t alarm_24v;
    uint8_t alarm_temp_high;
    uint8_t alarm_temp_low;
    uint8_t cover_open;
    uint8_t pump_high;
    uint8_t pump_low;
    uint8_t tracing;
} output_values_t;

#endif
