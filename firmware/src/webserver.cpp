#include "webserver.h"
#include "scoring.h"
#include "config.h"

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Max shots to keep in RAM for the API
#define API_MAX_SHOTS 200

static AsyncWebServer s_server(80);

struct ApiShot {
    float x, y;
    int   ring, shotNumber, runningTotal;
};

static ApiShot s_shots[API_MAX_SHOTS];
static int     s_shotCount = 0;

// ─── JSON helpers ─────────────────────────────────────────────────────────────

static String buildShotsJson()
{
    String json = "[";
    for (int i = 0; i < s_shotCount; ++i) {
        if (i > 0) json += ",";
        char buf[128];
        snprintf(buf, sizeof(buf),
            "{\"shot\":%d,\"x\":%.1f,\"y\":%.1f,\"ring\":%d,\"total\":%d}",
            s_shots[i].shotNumber,
            s_shots[i].x,
            s_shots[i].y,
            s_shots[i].ring,
            s_shots[i].runningTotal);
        json += buf;
    }
    json += "]";
    return json;
}

static String buildStatsJson()
{
    const SessionStats &st = scoring_get_stats();
    char buf[128];
    snprintf(buf, sizeof(buf),
        "{\"shots\":%d,\"total\":%d,\"groupRadius\":%.1f}",
        st.shotCount, st.totalScore, st.groupRadius);
    return String(buf);
}

// ─── Public API ───────────────────────────────────────────────────────────────

void webserver_init()
{
    // Start access point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
    Serial.print("[WiFi] AP IP: ");
    Serial.println(WiFi.softAPIP());

    // Mount LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] LittleFS mount failed");
    }

    // Serve dashboard
    s_server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
        req->send(LittleFS, "/index.html", "text/html");
    });

    // Shot log API
    s_server.on("/api/shots", HTTP_GET, [](AsyncWebServerRequest *req) {
        req->send(200, "application/json", buildShotsJson());
    });

    // Stats API
    s_server.on("/api/stats", HTTP_GET, [](AsyncWebServerRequest *req) {
        req->send(200, "application/json", buildStatsJson());
    });

    // Reset session
    s_server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *req) {
        scoring_init();
        webserver_clear_shots();
        req->send(204);
    });

    // 404
    s_server.onNotFound([](AsyncWebServerRequest *req) {
        req->send(404, "text/plain", "Not found");
    });

    s_server.begin();
    Serial.println("[Web] Server started");
}

void webserver_push_shot(const ShotResult &shot)
{
    if (s_shotCount >= API_MAX_SHOTS) return;
    ApiShot &a   = s_shots[s_shotCount++];
    a.x          = shot.x;
    a.y          = shot.y;
    a.ring       = shot.ring;
    a.shotNumber = shot.shotNumber;
    a.runningTotal = shot.runningTotal;
}

void webserver_clear_shots()
{
    s_shotCount = 0;
}
