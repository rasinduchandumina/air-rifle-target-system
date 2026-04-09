#include "triangulation.h"
#include "config.h"
#include <math.h>

// ─── Sensor positions (mm, relative to board centre) ─────────────────────────

static const float SX[4] = {
    SENSOR_0_X, SENSOR_1_X, SENSOR_2_X, SENSOR_3_X
};
static const float SY[4] = {
    SENSOR_0_Y, SENSOR_1_Y, SENSOR_2_Y, SENSOR_3_Y
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

static inline float dist2D(float ax, float ay, float bx, float by)
{
    float dx = ax - bx;
    float dy = ay - by;
    return sqrtf(dx * dx + dy * dy);
}

// ─── Triangulation ────────────────────────────────────────────────────────────

ImpactPoint triangulate(const uint64_t t[4])
{
    ImpactPoint result = {0.0f, 0.0f, false};

    // ── 1. Find reference sensor (earliest arrival) ───────────────────────────
    int ref = 0;
    for (int i = 1; i < 4; ++i) {
        if (t[i] < t[ref]) ref = i;
    }

    // d[i] = dist(P, S_i) - dist(P, S_ref) = c * (t[i] - t[ref])
    float d[4];
    for (int i = 0; i < 4; ++i) {
        d[i] = SPEED_OF_SOUND_MM_US * (float)(int64_t)(t[i] - t[ref]);
    }
    // d[ref] == 0 by definition

    // ── 2. Collect the three non-reference sensor indices ─────────────────────
    int idx[3];
    int k = 0;
    for (int i = 0; i < 4; ++i) {
        if (i != ref) idx[k++] = i;
    }

    // ── 3. Initial closed-form estimate (linearised, ignoring r_ref) ─────────
    // From hyperbola equations, subtracting pairs gives:
    //   2*(SX[ref]-SX[i])*x + 2*(SY[ref]-SY[i])*y = d[i]^2
    //       + SX[ref]^2 - SX[i]^2 + SY[ref]^2 - SY[i]^2
    float A[2][2], b_vec[2];
    for (int row = 0; row < 2; ++row) {
        int i        = idx[row];
        A[row][0]    = 2.0f * (SX[ref] - SX[i]);
        A[row][1]    = 2.0f * (SY[ref] - SY[i]);
        b_vec[row]   = d[i] * d[i]
                     + SX[ref] * SX[ref] - SX[i] * SX[i]
                     + SY[ref] * SY[ref] - SY[i] * SY[i];
    }

    float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
    if (fabsf(det) < 1e-6f) {
        return result; // degenerate geometry
    }

    float x = (b_vec[0] * A[1][1] - b_vec[1] * A[0][1]) / det;
    float y = (A[0][0] * b_vec[1] - A[1][0] * b_vec[0]) / det;

    // ── 4. Gauss-Newton refinement (3 iterations over all 3 equations) ────────
    for (int iter = 0; iter < 3; ++iter) {
        float r_ref = dist2D(x, y, SX[ref], SY[ref]);
        if (r_ref < 1e-6f) break;

        // Build 3x2 Jacobian and residual vector
        float Jt_J[2][2] = {{0}};
        float Jt_r[2]    = {0};

        for (int row = 0; row < 3; ++row) {
            int   i   = idx[row];
            float ri  = dist2D(x, y, SX[i], SY[i]);
            if (ri < 1e-9f) continue;

            // residual: f = r_i - r_ref - d[i]
            float f = ri - r_ref - d[i];

            // Jacobian row: [df/dx, df/dy]
            float jx = (x - SX[i]) / ri - (x - SX[ref]) / r_ref;
            float jy = (y - SY[i]) / ri - (y - SY[ref]) / r_ref;

            Jt_J[0][0] += jx * jx;
            Jt_J[0][1] += jx * jy;
            Jt_J[1][0] += jy * jx;
            Jt_J[1][1] += jy * jy;
            Jt_r[0]    += jx * f;
            Jt_r[1]    += jy * f;
        }

        float det2 = Jt_J[0][0] * Jt_J[1][1] - Jt_J[0][1] * Jt_J[1][0];
        if (fabsf(det2) < 1e-10f) break;

        float dx_step = -(Jt_r[0] * Jt_J[1][1] - Jt_r[1] * Jt_J[0][1]) / det2;
        float dy_step = -(Jt_J[0][0] * Jt_r[1] - Jt_J[1][0] * Jt_r[0]) / det2;

        x += dx_step;
        y += dy_step;

        if (fabsf(dx_step) < 0.01f && fabsf(dy_step) < 0.01f) break;
    }

    // ── 5. Clamp to board boundaries ──────────────────────────────────────────
    float maxR = sqrtf(BOARD_HALF_W_MM * BOARD_HALF_W_MM +
                       BOARD_HALF_H_MM * BOARD_HALF_H_MM);
    if (dist2D(x, y, 0.0f, 0.0f) > maxR * 1.5f) {
        return result; // wildly out of range — triangulation failed
    }

    result.x     = x;
    result.y     = y;
    result.valid = true;
    return result;
}
