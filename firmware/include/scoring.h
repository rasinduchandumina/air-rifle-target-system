#pragma once
#include "triangulation.h"

/**
 * Per-shot scoring result.
 */
struct ShotResult {
    float   x;          ///< x coordinate (mm)
    float   y;          ///< y coordinate (mm)
    int     ring;       ///< scoring ring value 1–10, or 0 = miss
    int     shotNumber; ///< 1-based index within the current session
    int     runningTotal;
};

/**
 * Persistent session statistics.
 */
struct SessionStats {
    int   shotCount;
    int   totalScore;
    float groupRadius;  ///< radius of tightest enclosing circle of all shots (mm)
};

/**
 * Initialise (or reset) the scoring session.
 */
void scoring_init();

/**
 * Compute ring score for a hit at (x, y) and update session statistics.
 * @param  pt   Impact coordinates in mm.
 * @return      Populated ShotResult.
 */
ShotResult scoring_evaluate(const ImpactPoint &pt);

/**
 * Return a read-only snapshot of the current session statistics.
 */
const SessionStats &scoring_get_stats();
