#include <stdint.h>
#include "logic.h"

#define TEMP_O_LOW  5.0
#define TEMP_O_HIGH 18.0
#define TEMP_W_LOW  2.0
#define TEMP_W_HIGH 30.0
#define LUX_HIGH    1600
#define HYSTERESIS  1

static uint8_t validate_pump_high(input_values_t* inputs, uint8_t value)
{
    uint8_t pump_high = value;

    if (inputs->timer) {
        pump_high = 1;
    }
    else if (inputs->outside_temp < TEMP_O_LOW && inputs->water_temp < TEMP_W_LOW) {
        pump_high = 1;
    }

    if ((inputs->outside_temp >= (TEMP_O_LOW + HYSTERESIS) || inputs->water_temp >= (TEMP_W_LOW + HYSTERESIS)) && !inputs->timer) {
        pump_high = 0;
    }

    return pump_high;
}

static uint8_t validate_pump_low(input_values_t* inputs, uint8_t value, uint8_t pump_high)
{
    uint8_t pump_low = value;

    if (pump_high) {
        return 3;  // Force pump low when pump_high is active
    }

    if (inputs->outside_temp < TEMP_O_LOW) {
        pump_low = 1;
    }
    if (inputs->outside_temp > TEMP_O_HIGH && inputs->lux > LUX_HIGH) {
        pump_low = 2;
    }

    if (pump_low == 1 && inputs->outside_temp >= (TEMP_O_LOW + HYSTERESIS)) {
        pump_low = 0;
    }
    if (pump_low == 2) {
        if (inputs->outside_temp <= (TEMP_O_HIGH - HYSTERESIS) || (inputs->lux <= (LUX_HIGH - (100*HYSTERESIS)))) {
            pump_low = 0;
        }
    }

    return pump_low;
}

static uint8_t validate_tracing(input_values_t* inputs)
{
    uint8_t tracing = 0;

    if (inputs->outside_temp < TEMP_O_LOW && inputs->water_temp < TEMP_W_LOW) {
        tracing = 1;
    }

    return tracing;
}

static uint8_t validate_cover_open(input_values_t* inputs, uint8_t value)
{
    uint8_t cover_open = value;

    // When pump is inactive clear alarm
    if (!inputs->pump_active) {
        return 0;
    }

    if (inputs->water_temp >= TEMP_W_HIGH) {
        cover_open = 1;
    }

    if (inputs->water_temp < (TEMP_W_HIGH - HYSTERESIS)) {
        cover_open = 0;
    }

    return cover_open;
}

static uint8_t validate_alarm_temp_low(input_values_t* inputs, uint8_t value)
{
    uint8_t alarm = value;

    // When pump is inactive clear alarm
    if (!inputs->pump_active) {
        return 0;
    }

    if (inputs->water_temp < TEMP_W_LOW) {
        alarm = 1;
    }

    if (inputs->water_temp >= (TEMP_W_LOW + HYSTERESIS)) {
        alarm = 0;
    }

    return alarm;
}

int run_logic(input_values_t* inputs, output_values_t* outputs)
{
    outputs->pump_high = validate_pump_high(inputs, outputs->pump_high);
    outputs->pump_low = validate_pump_low(inputs, outputs->pump_low, outputs->pump_high);
    outputs->tracing = validate_tracing(inputs);
    outputs->cover_open = validate_cover_open(inputs, 0);
    outputs->alarm_temp_high = validate_cover_open(inputs, outputs->alarm_temp_high);
    outputs->alarm_temp_low = validate_alarm_temp_low(inputs, outputs->alarm_temp_low);

    outputs->alarm_230v = inputs->power_230v ? 0 : 1;
    outputs->alarm_24v = inputs->power_24v ? 0 : 1;

    return 0;
}
