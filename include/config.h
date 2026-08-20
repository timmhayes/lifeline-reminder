#pragma once

// ============================================================================
// WiFi Configuration
// ============================================================================
#define WIFI_SSID       "ADD_SSID_HERE"
#define WIFI_PASSWORD   "ADD_PASSWORD_HERE"

// Optional backup WiFi credentials for a second location.
// Leave these empty if you only want to use the primary network.
#define WIFI_BACKUP_SSID      "ADD_SECONDARY_SSID_HERE"
#define WIFI_BACKUP_PASSWORD  "ADD_SECONDARY_PASSWORD_HERE"

// ============================================================================
// Display & UI Configuration
// ============================================================================
// Colors (RGB565 hex format)
#define COLOR_BACKGROUND  0x000000  // Black
#define COLOR_TIME_TEXT   0x00FFFF  // Cyan
#define COLOR_DATE_TEXT   0xFFFF00  // Yellow

// Dementia-friendly UI state colors (RGB565)
#define UI_ALARM_TEXT_COLOR    0xFFD0  // Warm amber
#define UI_SUCCESS_TEXT_COLOR  0x87F0  // Soft green
#define UI_SUPPORT_TEXT_COLOR  0xFFFF  // White

// Time Label Configuration
#define TIME_LABEL_OFFSET_Y  -20
#define TIME_LABEL_FONT      (&lv_font_montserrat_28)

// Date Label Configuration
#define DATE_LABEL_OFFSET_Y  10
#define DATE_LABEL_FONT      (&lv_font_montserrat_22)

// ============================================================================
// Time Synchronization Configuration
// ============================================================================
// NTP timezone string (EST/EDT with automatic DST)
#define NTP_TIMEZONE     "EST5EDT,M3.2.0,M11.1.0"
#define NTP_SERVER_1     "pool.ntp.org"
#define NTP_SERVER_2     "time.nist.gov"

// ============================================================================
// Timing Configuration
// ============================================================================
// WiFi connection check interval (milliseconds)
#define WIFI_CHECK_INTERVAL   500

// Maximum time spent on each WiFi credential before trying the next one (milliseconds)
#define WIFI_CONNECT_ATTEMPT_MS  15000

// Time display update interval (milliseconds)
#define TIME_UPDATE_INTERVAL  1000

// Brief acknowledgement shown when the button is clicked after the daily check-in is already complete.
#define UI_ACKNOWLEDGEMENT_MS  2000

// LVGL tick period (milliseconds)
#define LVGL_TICK_PERIOD      5

// Loop delay (milliseconds)
#define LOOP_DELAY            5

// getLocalTime timeout (deciseconds, 10 = 1 second)
#define GET_LOCAL_TIME_TIMEOUT  10

// ============================================================================
// Safety Check-In Hardware Configuration
// ============================================================================
#define PHOTORESISTOR_PIN          1
#define NEOPIXEL_PIN               23
#define NEOPIXEL_COUNT             8

// ============================================================================
// Daily State Schedule
// ============================================================================
#define RESET_HOUR                 0
#define RESET_MINUTE               0
#define ALARM_HOUR                 5
#define ALARM_MINUTE               0

// ============================================================================
// Photoresistor Flash Detection Tuning
// ============================================================================
#define SENSOR_SAMPLE_INTERVAL_MS  30
#define SENSOR_BASELINE_WINDOW     16
#define SENSOR_WARMUP_SAMPLES      20
#define SENSOR_TRIGGER_DELTA       500
#define SENSOR_MIN_ABSOLUTE        400
#define SENSOR_DEBOUNCE_MS         1500

// ============================================================================
// NeoPixel Animation Timing
// ============================================================================
#define LED_ANIM_INTERVAL_MS       40
#define LED_SUCCESS_ON_MS          8000

// Dementia-friendly UI animation timing
#define UI_PHRASE_ROTATE_MS        5000
#define UI_BREATH_STEP_MS          50
#define UI_BREATH_STEP_AMOUNT      6
#define UI_MIN_OPACITY             140
#define UI_MAX_OPACITY             255
#define UI_SUCCESS_GLOW_MS         1200

// Dementia-friendly NeoPixel comfort settings
#define LED_IDLE_BRIGHTNESS        8
#define LED_ACTIVE_BRIGHTNESS      32
#define LED_SUCCESS_BRIGHTNESS     40
#define LED_SUCCESS_CELEBRATE_MS   5000

// ============================================================================
// Local Log + Web Dashboard
// ============================================================================
#define SUCCESS_LOG_PATH           "/success_log.txt"
#define WEB_SERVER_PORT            80

// ============================================================================
// Time & Date Format Strings
// ============================================================================
#define TIME_FORMAT_STR   "%I:%M:%S %p"      // 12:34:56 PM
#define DATE_FORMAT_STR   "%A, %b %d, %Y"    // Monday, Jul 14, 2026
