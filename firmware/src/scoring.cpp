#include "scoring.h"
#include "config.h"
#include <math.h>
#include <string.h>

// Ring boundary radii from centre (mm), innermost first
static const float RING_RADII[10] = {
    RING_10_R_MM, RING_9_R_MM, RING_8_R_MM, RING_7_R_MM, RING_6_R_MM,
    RING_5_R_MM,  RING_4_R_MM, RING_3_R_MM, RING_2_R_MM, RING_1_R_MM
};

// Maximum shots stored per session
#define MAX_SHOTS 200

static SessionStats s_stats;
static float        s_shotX[MAX_SHOTS];
static float        s_shotY[MAX_SHOTS];

// ─── Helpers ──────────────────────────────────────────────────────────────────

static float shotGroupRadius()
{
    if (s_stats.shotCount < 2) return 0.0f;

    // Compute centroid
    float cx = 0.0f, cy = 0.0f;
    for (int i = 0; i < s_stats.shotCount; ++i) {
        cx += s_shotX[i];
        cy += s_shotY[i];
    }
    cx /= (float)s_stats.shotCount;
    cy /= (float)s_stats.shotCount;

    // Radius = max distance from centroid to any shot
    float maxR = 0.0f;
    for (int i = 0; i < s_stats.shotCount; ++i) {
        float dx = s_shotX[i] - cx;
        float dy = s_shotY[i] - cy;
        float r  = sqrtf(dx * dx + dy * dy);
        if (r > maxR) maxR = r;
    }
    return maxR;
}

// ─── Public API ───────────────────────────────────────────────────────────────

void scoring_init()
{
    memset(&s_stats, 0, sizeof(s_stats));
    memset(s_shotX, 0, sizeof(s_shotX));
    memset(s_shotY, 0, sizeof(s_shotY));
}

ShotResult scoring_evaluate(const ImpactPoint &pt)
{
    ShotResult res;
    res.x      = pt.x;
    res.y      = pt.y;
    res.ring   = 0; // miss by default

    // Determine ring value
    float dist = sqrtf(pt.x * pt.x + pt.y * pt.y);
    for (int ring = 10; ring >= 1; --ring) {
        if (dist <= RING_RADII[10 - ring]) {
            res.ring = ring;
            break;
        }
    }

    // Update session
    int idx = s_stats.shotCount;
    if (idx < MAX_SHOTS) {
        s_shotX[idx] = pt.x;
        s_shotY[idx] = pt.y;
    }
    s_stats.shotCount++;
    s_stats.totalScore   += res.ring;
    s_stats.groupRadius   = shotGroupRadius();

    res.shotNumber   = s_stats.shotCount;
    res.runningTotal = s_stats.totalScore;
    return res;
}

const SessionStats &scoring_get_stats()
{
    return s_stats;
}
