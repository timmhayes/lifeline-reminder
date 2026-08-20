#include "led_status.h"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "config.h"

namespace {
Adafruit_NeoPixel g_strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
AppState g_state = AppState::IDLE;
uint32_t g_last_anim_ms = 0;
uint8_t g_anim_step = 0;
uint32_t g_success_start_ms = 0;

enum class SuccessPhase : uint8_t {
    SPARKLE = 0,
    DRIFT,
    HOLD,
};

SuccessPhase g_success_phase = SuccessPhase::SPARKLE;

void FillAndShow(uint32_t color) {
    for (uint16_t i = 0; i < g_strip.numPixels(); i++) {
        g_strip.setPixelColor(i, color);
    }
    g_strip.show();
}

uint32_t DimColor(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    const uint16_t scaled_r = static_cast<uint16_t>(r) * brightness / 255U;
    const uint16_t scaled_g = static_cast<uint16_t>(g) * brightness / 255U;
    const uint16_t scaled_b = static_cast<uint16_t>(b) * brightness / 255U;
    return g_strip.Color(static_cast<uint8_t>(scaled_r), static_cast<uint8_t>(scaled_g), static_cast<uint8_t>(scaled_b));
}

uint32_t Wheel(uint8_t pos, uint8_t brightness) {
    pos = 255 - pos;
    if (pos < 85) {
        return DimColor(static_cast<uint8_t>(255 - pos * 3), 0, static_cast<uint8_t>(pos * 3), brightness);
    }
    if (pos < 170) {
        pos -= 85;
        return DimColor(0, static_cast<uint8_t>(pos * 3), static_cast<uint8_t>(255 - pos * 3), brightness);
    }
    pos -= 170;
    return DimColor(static_cast<uint8_t>(pos * 3), static_cast<uint8_t>(255 - pos * 3), 0, brightness);
}

void SetAllOff(void) {
    FillAndShow(g_strip.Color(0, 0, 0));
}

void RenderAlarmHeartbeat(void) {
    const uint16_t scan_length = (NEOPIXEL_COUNT * 2U) - 2U;
    g_anim_step = static_cast<uint8_t>((g_anim_step + 1U) % scan_length);

    const int16_t scanner_pos = static_cast<int16_t>(g_anim_step);
    const int16_t center = (scanner_pos < static_cast<int16_t>(NEOPIXEL_COUNT))
        ? scanner_pos
        : (static_cast<int16_t>(NEOPIXEL_COUNT) * 2 - 1 - scanner_pos);

    for (uint16_t i = 0; i < g_strip.numPixels(); i++) {
        const int16_t distance = abs(static_cast<int16_t>(i) - center);
        uint8_t glow = 0;

        if (distance == 0) {
            glow = 255;
        } else if (distance == 1) {
            glow = 160;
        } else if (distance == 2) {
            glow = 70;
        }

        const uint8_t red = static_cast<uint8_t>(40U + (glow * 4U) / 5U);
        const uint8_t green = static_cast<uint8_t>(10U + (glow / 6U));
        g_strip.setPixelColor(i, DimColor(red, green, 0, LED_ACTIVE_BRIGHTNESS));
    }
    g_strip.show();
}

void RenderSuccessSparkle(void) {
    g_anim_step = static_cast<uint8_t>(g_anim_step + 11);
    for (uint16_t i = 0; i < g_strip.numPixels(); i++) {
        const uint8_t shimmer = static_cast<uint8_t>((g_anim_step * 7) + (i * 41));
        const uint8_t white = static_cast<uint8_t>(100 + (shimmer % 120));
        const uint8_t green = static_cast<uint8_t>(40 + (shimmer % 80));
        g_strip.setPixelColor(i, DimColor(white, static_cast<uint8_t>(white + green / 2), white / 3, LED_SUCCESS_BRIGHTNESS));
    }
    g_strip.show();
}

void RenderSuccessDrift(void) {
    g_anim_step = static_cast<uint8_t>(g_anim_step + 4);
    for (uint16_t i = 0; i < g_strip.numPixels(); i++) {
        const uint8_t hue = static_cast<uint8_t>(g_anim_step + (i * (256 / NEOPIXEL_COUNT)));
        g_strip.setPixelColor(i, Wheel(hue, LED_SUCCESS_BRIGHTNESS));
    }
    g_strip.show();
}

void RenderSuccessHold(void) {
    FillAndShow(DimColor(120, 255, 120, LED_SUCCESS_BRIGHTNESS));
}
}  // namespace

void LedStatus_Init(void) {
    g_strip.begin();
    g_strip.setBrightness(255);
    SetAllOff();
}

void LedStatus_SetState(AppState state) {
    if (g_state == state) {
        return;
    }

    g_state = state;

    if (g_state == AppState::IDLE) {
        SetAllOff();
        return;
    }

    if (g_state == AppState::ALARM_ACTIVE) {
        g_anim_step = 0;
        g_last_anim_ms = 0;
        return;
    }

    if (g_state == AppState::SUCCESS) {
        g_success_start_ms = millis();
        g_success_phase = SuccessPhase::SPARKLE;
        g_anim_step = 0;
        g_last_anim_ms = 0;
        RenderSuccessSparkle();
    }
}

void LedStatus_Service(void) {
    const uint32_t now = millis();

    if (g_state == AppState::IDLE) {
        return;
    }

    if (g_state == AppState::ALARM_ACTIVE) {
        if ((now - g_last_anim_ms) < LED_ANIM_INTERVAL_MS) {
            return;
        }
        g_last_anim_ms = now;

        RenderAlarmHeartbeat();
        return;
    }

    if ((now - g_last_anim_ms) < LED_ANIM_INTERVAL_MS) {
        return;
    }
    g_last_anim_ms = now;

    const uint32_t success_elapsed = now - g_success_start_ms;
    if (success_elapsed >= LED_SUCCESS_CELEBRATE_MS) {
        SetAllOff();
        return;
    }

    if (success_elapsed < 900) {
        g_success_phase = SuccessPhase::SPARKLE;
    } else if (success_elapsed < 3000) {
        g_success_phase = SuccessPhase::DRIFT;
    } else {
        g_success_phase = SuccessPhase::HOLD;
    }

    if (g_success_phase == SuccessPhase::SPARKLE) {
        RenderSuccessSparkle();
        return;
    }

    if (g_success_phase == SuccessPhase::DRIFT) {
        RenderSuccessDrift();
        return;
    }

    RenderSuccessHold();
}
