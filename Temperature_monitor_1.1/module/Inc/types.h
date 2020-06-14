#ifndef TYPES
#define TYPES

#include <stdint.h>

struct input_values_t {
    uint8_t power_230v;
    uint8_t power_24v;
    uint32_t lux;
    float water_temp;
    float outside_temp;
};

struct output_values_t {
    uint8_t pump_low;
    uint8_t pump_high;
    uint8_t tracing;
    uint8_t alarm_temp_low;
    uint8_t alarm_230v;
    uint8_t alarm_24v;
};

#endif
