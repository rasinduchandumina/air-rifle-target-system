#pragma once
#include "scoring.h"

/**
 * Initialise the OLED display (I²C, SSD1306).
 * Returns true if the display was found; false if absent or init failed.
 */
bool display_init();

/**
 * Show a welcome / idle splash screen.
 */
void display_show_idle();

/**
 * Show the result of the most recent shot plus the running session total.
 */
void display_show_shot(const ShotResult &shot);

/**
 * Show a full-screen "Session reset" message for ~1 s, then revert to idle.
 */
void display_show_reset();
