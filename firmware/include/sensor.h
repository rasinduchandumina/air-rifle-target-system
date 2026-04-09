#pragma once
#include <stdint.h>

// Number of piezo sensors
#define NUM_SENSORS 4

/**
 * Stores the µs timestamp at which each sensor first triggered.
 * A value of 0 means the sensor has not yet triggered.
 */
struct TriggerEvent {
    volatile uint64_t timestamps[NUM_SENSORS];  ///< esp_timer_get_time() values (µs)
    volatile bool     triggered[NUM_SENSORS];
    volatile uint8_t  count;                    ///< how many sensors have fired
};

/**
 * Initialise GPIO pins and ADC for all sensors.
 * Must be called once from setup().
 */
void sensor_init();

/**
 * Polling function — call from loop().
 * Reads ADC values, detects threshold crossings, and updates the shared
 * TriggerEvent.  Returns true when a new complete event is ready.
 */
bool sensor_poll(TriggerEvent &evt);

/**
 * Reset the trigger event so the system is ready for the next shot.
 */
void sensor_reset(TriggerEvent &evt);
