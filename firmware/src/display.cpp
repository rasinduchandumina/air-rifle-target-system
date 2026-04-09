#include "display.h"
#include "config.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 s_display(OLED_SCREEN_W, OLED_SCREEN_H, &Wire, -1);
static bool s_ready = false;

bool display_init()
{
#if !OLED_ENABLED
    return false;
#endif
    if (!s_display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        Serial.println("[OLED] SSD1306 not found");
        s_ready = false;
        return false;
    }
    s_ready = true;
    s_display.clearDisplay();
    s_display.display();
    return true;
}

void display_show_idle()
{
    if (!s_ready) return;
    s_display.clearDisplay();
    s_display.setTextSize(1);
    s_display.setTextColor(SSD1306_WHITE);
    s_display.setCursor(12, 10);
    s_display.println("Air Rifle Target");
    s_display.setCursor(28, 28);
    s_display.println("Ready to fire");
    s_display.setCursor(8, 46);
    s_display.print("AP: ");
    s_display.println(WIFI_AP_SSID);
    s_display.display();
}

void display_show_shot(const ShotResult &shot)
{
    if (!s_ready) return;
    s_display.clearDisplay();
    s_display.setTextSize(2);
    s_display.setTextColor(SSD1306_WHITE);

    // Ring value — large font
    s_display.setCursor(4, 4);
    s_display.print("Ring: ");
    s_display.print(shot.ring);

    s_display.setTextSize(1);
    s_display.setCursor(4, 34);
    s_display.print("Shot #");
    s_display.print(shot.shotNumber);

    s_display.setCursor(4, 46);
    s_display.print("Total: ");
    s_display.print(shot.runningTotal);

    s_display.setCursor(4, 56);
    char buf[24];
    snprintf(buf, sizeof(buf), "x:%.0f y:%.0f", shot.x, shot.y);
    s_display.print(buf);

    s_display.display();
}

void display_show_reset()
{
    if (!s_ready) return;
    s_display.clearDisplay();
    s_display.setTextSize(2);
    s_display.setTextColor(SSD1306_WHITE);
    s_display.setCursor(14, 20);
    s_display.println("Session");
    s_display.setCursor(30, 42);
    s_display.println("Reset");
    s_display.display();
    delay(1000);
    display_show_idle();
}
