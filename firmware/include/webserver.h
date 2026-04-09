#pragma once
#include "scoring.h"

/**
 * Initialise the Wi-Fi access point and start the async HTTP server.
 *
 * The server exposes:
 *   GET  /           → serves web/index.html from LittleFS
 *   GET  /api/shots  → JSON array of all shots this session
 *   GET  /api/stats  → JSON object with session statistics
 *   POST /api/reset  → resets the scoring session (returns 204)
 */
void webserver_init();

/**
 * Push a new shot to the internal shot log so that /api/shots reflects it.
 * Call this after scoring_evaluate().
 */
void webserver_push_shot(const ShotResult &shot);

/**
 * Clear the internal shot log (called when the session is reset).
 */
void webserver_clear_shots();
