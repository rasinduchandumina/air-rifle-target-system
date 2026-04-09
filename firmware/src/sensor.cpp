#include "sensor.h"
#include "config.h"
#include <Arduino.h>
#include <esp_timer.h>

// ─── Module-private state ─────────────────────────────────────────────────────

static const uint8_t SENSOR_PINS[NUM_SENSORS] = {
    SENSOR_PIN_0, SENSOR_PIN_1, SENSOR_PIN_2, SENSOR_PIN_3
};

// Timestamp of the first sensor that triggered in the current event.
// Used to enforce MAX_TRIGGER_WINDOW_US.
static uint64_t s_firstTriggerUs = 0;
static bool     s_eventActive    = false;

// ─── Public API ───────────────────────────────────────────────────────────────

void sensor_init()
{
    for (int i = 0; i < NUM_SENSORS; ++i) {
        pinMode(SENSOR_PINS[i], INPUT);
        analogSetPinAttenuation(SENSOR_PINS[i], ADC_11db); // 0–3.3 V range
    }
    analogReadResolution(12); // 12-bit → 0–4095
}

bool sensor_poll(TriggerEvent &evt)
{
    const uint64_t now = (uint64_t)esp_timer_get_time();

    // Check if the open window has expired without all sensors firing
    if (s_eventActive &&
        (now - s_firstTriggerUs) > MAX_TRIGGER_WINDOW_US)
    {
        // Treat sensors that never triggered as arriving last (now)
        for (int i = 0; i < NUM_SENSORS; ++i) {
            if (!evt.triggered[i]) {
                evt.timestamps[i] = now;
                evt.triggered[i]  = true;
                ++evt.count;
            }
        }
    }

    // Sample all un-triggered sensors
    for (int i = 0; i < NUM_SENSORS; ++i) {
        if (evt.triggered[i]) continue;

        int adc = analogRead(SENSOR_PINS[i]);
        if (adc >= IMPACT_THRESHOLD) {
            uint64_t ts = (uint64_t)esp_timer_get_time();
            evt.timestamps[i] = ts;
            evt.triggered[i]  = true;
            ++evt.count;

            if (!s_eventActive) {
                s_eventActive    = true;
                s_firstTriggerUs = ts;
            }
        }
    }

    // Return true once all four sensors have been recorded
    if (evt.count >= NUM_SENSORS) {
        s_eventActive = false;
        return true;
    }
    return false;
}

void sensor_reset(TriggerEvent &evt)
{
    for (int i = 0; i < NUM_SENSORS; ++i) {
        evt.timestamps[i] = 0;
        evt.triggered[i]  = false;
    }
    evt.count     = 0;
    s_eventActive = false;
    s_firstTriggerUs = 0;
}
