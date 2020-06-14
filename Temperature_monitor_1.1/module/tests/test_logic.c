#include "unity.h"
#include "types.h"
#include "logic.h"

input_values_t inputs;
output_values_t outputs;

void setUp()
{
    inputs.power_230v = 1;
    inputs.power_24v = 1;
    inputs.lux = 0;
    inputs.water_temp = 0;
    inputs.outside_temp = 0;

    outputs.pump_low = 0;
    outputs.pump_high = 0;
    outputs.tracing = 0;
    outputs.alarm_temp_low = 0;
    outputs.alarm_temp_high = 0;
    outputs.alarm_230v = 0;
    outputs.alarm_24v = 0;
    outputs.cover_open = 0;
}

void tearDown()
{
}

void test_low_outside_temp_triggers_pump(void)
{
    int ret = 0;

    // When outside temp is above 5c
    inputs.outside_temp = 6.0;

    // Pump_low should be active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.pump_low);

    // When outside temp is below 5c
    inputs.outside_temp = 4.0;

    // Pump_low should be active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.pump_low);

    // When outside temp is again above 5c
    inputs.outside_temp = 6.0;

    // Pump_low should be active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.pump_low);
}

void test_low_outside_temp_and_water_temp_triggers_pump(void)
{
    int ret = 0;

    // When outside temp is under 5c and water temp is under 2
    inputs.outside_temp = 4.0;
    inputs.water_temp = 1.0;

    // Pump_high and tracing should be active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.pump_high);
    TEST_ASSERT_EQUAL_INT(1, outputs.tracing);

    // When outside temp is above 5c and water temp is under 2
    inputs.outside_temp = 6.0;
    inputs.water_temp = 1.0;

    // Pump_high and tracing should be inactive
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.pump_high);
    TEST_ASSERT_EQUAL_INT(0, outputs.tracing);
}

void test_low_water_temp_should_trigger_alarm(void)
{
    int ret = 0;

    // When water temp is under 2c
    inputs.water_temp = 1.0;

    // An alarm should be triggered
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.alarm_temp_low);

    // When water temp is above 2c
    inputs.water_temp = 3.0;

    // Alarm should be cleared
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.alarm_temp_low);
}

void test_no_230v_should_trigger_alarm(void)
{
    int ret = 0;

    // When 230v is active
    inputs.power_230v = 1;

    // No alarm should be triggered
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.alarm_230v);

    // When 230v is gone
    inputs.power_230v = 0;

    // An alarm should be triggered
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.alarm_230v);
}

void test_no_24v_should_trigger_alarm(void)
{
    int ret = 0;

    // When 24v is active
    inputs.power_24v = 1;

    // No alarm should be triggered
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.alarm_24v);

    // When 24v is gone
    inputs.power_24v = 0;

    // An alarm should be triggered
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.alarm_24v);
}

void test_high_outside_temp_and_lux_starts_pump(void)
{
    int ret = 0;

    // When outside temp is higher then 18c and light intensity is more then 1600lux
    inputs.outside_temp = 18.5;
    inputs.lux = 1601;

    // pump low should be active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.pump_low);

    // When outside temp is lower then 18c and light intensity is more then 1600lux
    inputs.outside_temp = 17.5;
    inputs.lux = 1600;

    // pump low should be inactive
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.pump_low);

    // When outside temp is higher then 18c and light intensity is less then 1600lux
    inputs.outside_temp = 18.5;
    inputs.lux = 1599;

    // pump low should be inactive
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.pump_low);
}

void test_high_water_temp_opens_cover_and_triggers_alarm(void)
{
    int ret = 0;

    // When the water temp is 30c or higher
    inputs.water_temp = 30.0;

    // The pool cover should open
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.cover_open);
    TEST_ASSERT_EQUAL_INT(1, outputs.alarm_temp_high);

    // When the water temp is lower then 30c
    inputs.water_temp = 29.9;

    // The pool cover should close
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.cover_open);
    TEST_ASSERT_EQUAL_INT(0, outputs.alarm_temp_high);
}

void test_timer_starts_pump_high(void)
{
    int ret = 0;

    // When timer is high
    inputs.timer = 1;

    // The pump is active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.pump_high);

    // When timer is low and other conditions are abnormal (cold)
    inputs.timer = 0;
    inputs.outside_temp = 0;
    inputs.water_temp = 0;

    // The pump is still active
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(1, outputs.pump_high);

    // When timer is low and other conditions are normal
    inputs.timer = 0;
    inputs.outside_temp = 20.0;
    inputs.water_temp = 20.0;

    // The pump is inactive
    ret = run_logic(&inputs, &outputs);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(0, outputs.pump_high);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_low_outside_temp_triggers_pump);
    RUN_TEST(test_low_outside_temp_and_water_temp_triggers_pump);
    RUN_TEST(test_low_water_temp_should_trigger_alarm);
    RUN_TEST(test_no_230v_should_trigger_alarm);
    RUN_TEST(test_no_24v_should_trigger_alarm);
    RUN_TEST(test_high_outside_temp_and_lux_starts_pump);
    RUN_TEST(test_high_water_temp_opens_cover_and_triggers_alarm);
    RUN_TEST(test_timer_starts_pump_high);

    return UNITY_END();
}
