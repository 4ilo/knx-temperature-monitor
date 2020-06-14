#include "logic.h"

int run_logic(struct input_values_t* inputs, struct output_values_t* outputs)
{
    if (inputs->outside_temp < 5.0) {
        outputs->pump_low = 1;

        if (inputs->water_temp < 2.0) {
            outputs->pump_high = 1;
            outputs->tracing = 1;
        }
        else {
            outputs->pump_high = 0;
            outputs->tracing = 0;
        }
    }
    else {
        outputs->pump_low = 0;
        outputs->pump_high = 0;
        outputs->tracing = 0;
    }

    // Generate alarm when water is to cold
    if (inputs->water_temp < 2.0) {
        outputs->alarm_temp_low = 1;
    }
    else {
        outputs->alarm_temp_low = 0;
    }

    outputs->alarm_230v = inputs->power_230v ? 0 : 1;

    return 0;
}
