#pragma once

// ─── Target board geometry ────────────────────────────────────────────────────
// All dimensions in millimetres, origin = board centre.
// Sensors are placed at the four corners of the board.

// Half-width and half-height of the target board (mm)
#define BOARD_HALF_W_MM   150.0f   // total width  = 300 mm
#define BOARD_HALF_H_MM   150.0f   // total height = 300 mm

// Sensor positions (x, y) in mm relative to board centre
//   S0 = bottom-left   S1 = bottom-right
//   S2 = top-right     S3 = top-left
#define SENSOR_0_X  (-BOARD_HALF_W_MM)
#define SENSOR_0_Y  (-BOARD_HALF_H_MM)
#define SENSOR_1_X  ( BOARD_HALF_W_MM)
#define SENSOR_1_Y  (-BOARD_HALF_H_MM)
#define SENSOR_2_X  ( BOARD_HALF_W_MM)
#define SENSOR_2_Y  ( BOARD_HALF_H_MM)
#define SENSOR_3_X  (-BOARD_HALF_W_MM)
#define SENSOR_3_Y  ( BOARD_HALF_H_MM)

// ─── Acoustics ────────────────────────────────────────────────────────────────
// Speed of sound at ~20 °C in mm/µs (343 000 mm/s → 0.343 mm/µs)
#define SPEED_OF_SOUND_MM_US  0.343f

// ─── Sensor GPIO pins (ADC-capable) ───────────────────────────────────────────
#define SENSOR_PIN_0  34   // GPIO34 — input only
#define SENSOR_PIN_1  35   // GPIO35 — input only
#define SENSOR_PIN_2  32
#define SENSOR_PIN_3  33

// Amplitude threshold to declare an impact (0–4095, 12-bit ADC)
#define IMPACT_THRESHOLD  400

// Guard window after first trigger: ignore further triggers (µs)
#define DEBOUNCE_WINDOW_US  3000

// Maximum time window within which all 4 sensors must trigger (µs)
// = board diagonal / speed_of_sound  ≈  424 mm / 0.343 ≈ 1236 µs → round up
#define MAX_TRIGGER_WINDOW_US  2000

// ─── Scoring ring radii (mm from centre) ─────────────────────────────────────
// Standard 10-ring air-rifle target scaled to the board size.
// Ring 10 (innermost) → ring 1 (outermost)
#define RING_10_R_MM   7.5f
#define RING_9_R_MM   17.5f
#define RING_8_R_MM   27.5f
#define RING_7_R_MM   37.5f
#define RING_6_R_MM   47.5f
#define RING_5_R_MM   57.5f
#define RING_4_R_MM   67.5f
#define RING_3_R_MM   77.5f
#define RING_2_R_MM   87.5f
#define RING_1_R_MM  100.0f

// ─── Wi-Fi access point ───────────────────────────────────────────────────────
#define WIFI_AP_SSID  "AirRifleTarget"
#define WIFI_AP_PASS  "target1234"
#define WIFI_AP_IP    "192.168.4.1"

// ─── OLED display ─────────────────────────────────────────────────────────────
#define OLED_ENABLED     true
#define OLED_I2C_ADDR    0x3C
#define OLED_SCREEN_W    128
#define OLED_SCREEN_H    64

// ─── Serial ───────────────────────────────────────────────────────────────────
#define SERIAL_BAUD  115200
