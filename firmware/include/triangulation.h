#pragma once
#include <stdint.h>
#include <stdbool.h>

/**
 * (x, y) impact coordinates in mm relative to board centre.
 * valid = false when triangulation failed to converge.
 */
struct ImpactPoint {
    float x;
    float y;
    bool  valid;
};

/**
 * Compute the 2-D impact position using Time-Difference-Of-Arrival (TDOA)
 * from four sensors whose positions are defined in config.h.
 *
 * @param t   Array of four arrival timestamps in microseconds (µs).
 *            t[i] = 0 means sensor i did not trigger (treated as the last).
 * @return    ImpactPoint with coordinates in mm; valid=false on failure.
 *
 * Algorithm
 * ---------
 * We pick sensor 0 as the reference and form three TDOA equations:
 *   d_i0 = c * (t[i] - t[0])   for i = 1,2,3
 * where c = speed of sound (mm/µs).
 *
 * Each equation d_i0 = dist(P, Si) - dist(P, S0) defines a hyperbola.
 * We linearise by subtracting pairs of these equations, producing a
 * 2×2 linear system that is solved in closed form.  A final nonlinear
 * refinement step (one Gauss-Newton iteration) corrects the linearisation
 * error.
 */
ImpactPoint triangulate(const uint64_t t[4]);
