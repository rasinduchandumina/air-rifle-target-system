/**
 * Air Rifle Target System — Main entry point
 *
 * Flow:
 *  setup()  → initialise peripherals, Wi-Fi, web server, OLED
 *  loop()   → poll sensors; on complete event: triangulate, score, report
 */

#include <Arduino.h>
#include "config.h"
#include "sensor.h"
#include "triangulation.h"
#include "scoring.h"
#include "webserver.h"
#include "display.h"

static TriggerEvent g_evt;

void setup()
{
    Serial.begin(SERIAL_BAUD);
    Serial.println("\n=== Air Rifle Target System ===");

    sensor_init();
    sensor_reset(g_evt);

    scoring_init();

    webserver_init();

    if (display_init()) {
        Serial.println("[OLED] Display initialised");
    }

    display_show_idle();
    Serial.println("[SYS] Ready");
}

void loop()
{
    if (!sensor_poll(g_evt)) {
        // No complete event yet — yield
        delay(1);
        return;
    }

    // ── We have timestamps from all four sensors ──────────────────────────────
    Serial.println("[EVT] Impact detected");
    for (int i = 0; i < NUM_SENSORS; ++i) {
        Serial.printf("  S%d: %llu us\n", i, g_evt.timestamps[i]);
    }

    // ── Triangulate ───────────────────────────────────────────────────────────
    ImpactPoint pt = triangulate(g_evt.timestamps);

    if (!pt.valid) {
        Serial.println("[TRI] Triangulation failed — discarding event");
        sensor_reset(g_evt);
        return;
    }
    Serial.printf("[TRI] x=%.1f mm  y=%.1f mm\n", pt.x, pt.y);

    // ── Score ─────────────────────────────────────────────────────────────────
    ShotResult shot = scoring_evaluate(pt);
    Serial.printf("[SCR] Ring=%d  Shot#=%d  Total=%d  Group=%.1f mm\n",
        shot.ring, shot.shotNumber, shot.runningTotal,
        scoring_get_stats().groupRadius);

    // ── Report ────────────────────────────────────────────────────────────────
    webserver_push_shot(shot);
    display_show_shot(shot);

    // ── Ready for next shot ───────────────────────────────────────────────────
    sensor_reset(g_evt);

    // Brief dead-time to avoid re-triggering on reverberations
    delay(DEBOUNCE_WINDOW_US / 1000 + 10);
}
