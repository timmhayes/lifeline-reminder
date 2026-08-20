#include "ui_status.h"

#include <Arduino.h>

#include "config.h"

namespace {
lv_obj_t* g_main_label = nullptr;
lv_obj_t* g_support_label = nullptr;
AppState g_ui_state = AppState::IDLE;
uint32_t g_state_start_ms = 0;
uint32_t g_last_phrase_ms = 0;
uint32_t g_last_anim_ms = 0;
uint8_t g_alarm_phrase_index = 0;
uint8_t g_success_phrase_index = 0;
uint8_t g_breath_step = 0;
bool g_ack_active = false;
uint32_t g_ack_end_ms = 0;

const char* const kAlarmMainPhrases[] = {
    "It's button time!",
    "Press the red button",
    "Check-in now..."
};

const char* const kAlarmSupportPhrases[] = {
    "Press every morning",
    "Before 10:00 AM",
    "And I'll stop nagging!"
};

const char* const kSuccessMainPhrases[] = {
    "Great job today",
    "You are all set!",
    "You're all checked in"
};

const char* const kSuccessSupportPhrases[] = {
    "Thank you",
    "That was perfect",
    "Next check-in tomorrow"
};

const char* const kAcknowledgementMainText = "Already checked in today";
const char* const kAcknowledgementSupportText = "Thank you for pressing it";

constexpr size_t kAlarmPhraseCount = sizeof(kAlarmMainPhrases) / sizeof(kAlarmMainPhrases[0]);
constexpr size_t kSuccessPhraseCount = sizeof(kSuccessMainPhrases) / sizeof(kSuccessMainPhrases[0]);

uint8_t TriangleWaveStep(uint8_t step) {
    if (step < 128) {
        return static_cast<uint8_t>(step * 2);
    }
    return static_cast<uint8_t>(255 - ((step - 128) * 2));
}

void ApplyAlarmPhrase(void) {
    if (g_main_label != nullptr) {
        lv_label_set_text(g_main_label, kAlarmMainPhrases[g_alarm_phrase_index]);
    }
    if (g_support_label != nullptr) {
        lv_label_set_text(g_support_label, kAlarmSupportPhrases[g_alarm_phrase_index]);
    }
}

void ShowIdle(void) {
    if (g_main_label != nullptr) {
        lv_obj_set_style_text_opa(g_main_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_color(g_main_label, lv_color_hex(COLOR_TIME_TEXT), 0);
    }

    if (g_support_label != nullptr) {
        lv_label_set_text(g_support_label, "");
        lv_obj_set_style_text_opa(g_support_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_color(g_support_label, lv_color_hex(COLOR_DATE_TEXT), 0);
    }
}

void ShowAlarmPrompt(void) {
    g_alarm_phrase_index = 0;
    ApplyAlarmPhrase();

    if (g_main_label != nullptr) {
        lv_obj_set_style_text_color(g_main_label, lv_color_hex(UI_ALARM_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(g_main_label, LV_OPA_COVER, 0);
    }
    if (g_support_label != nullptr) {
        lv_obj_set_style_text_color(g_support_label, lv_color_hex(UI_SUPPORT_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(g_support_label, LV_OPA_COVER, 0);
    }
}

void ShowSuccess(void) {
    g_success_phrase_index = 0;
    g_ack_active = false;

    if (g_main_label != nullptr) {
        lv_label_set_text(g_main_label, kSuccessMainPhrases[g_success_phrase_index]);
        lv_obj_set_style_text_color(g_main_label, lv_color_hex(UI_SUCCESS_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(g_main_label, LV_OPA_COVER, 0);
    }

    if (g_support_label != nullptr) {
        lv_label_set_text(g_support_label, kSuccessSupportPhrases[g_success_phrase_index]);
        lv_obj_set_style_text_color(g_support_label, lv_color_hex(UI_SUPPORT_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(g_support_label, LV_OPA_COVER, 0);
    }
}

void ShowAcknowledgement(void) {
    g_ack_active = true;
    g_ack_end_ms = millis() + UI_ACKNOWLEDGEMENT_MS;

    if (g_main_label != nullptr) {
        lv_label_set_text(g_main_label, kAcknowledgementMainText);
        lv_obj_set_style_text_color(g_main_label, lv_color_hex(UI_SUCCESS_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(g_main_label, LV_OPA_COVER, 0);
    }

    if (g_support_label != nullptr) {
        lv_label_set_text(g_support_label, kAcknowledgementSupportText);
        lv_obj_set_style_text_color(g_support_label, lv_color_hex(UI_SUPPORT_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(g_support_label, LV_OPA_COVER, 0);
    }
}
}  // namespace

void UiStatus_Init(lv_obj_t* main_label, lv_obj_t* support_label) {
    g_main_label = main_label;
    g_support_label = support_label;
    g_ui_state = AppState::IDLE;
}

void UiStatus_ApplyState(AppState state) {
    g_ui_state = state;
    g_state_start_ms = millis();
    g_last_phrase_ms = 0;
    g_last_anim_ms = 0;
    g_breath_step = 0;
    g_ack_active = false;

    if (g_ui_state == AppState::IDLE) {
        ShowIdle();
        return;
    }

    if (g_ui_state == AppState::ALARM_ACTIVE) {
        ShowAlarmPrompt();
        return;
    }

    ShowSuccess();
}

void UiStatus_UpdateClock(const char* time_text, const char* date_text) {
    if (g_ui_state != AppState::IDLE) {
        return;
    }

    if (g_main_label != nullptr) {
        lv_label_set_text(g_main_label, time_text);
    }

    if (g_support_label != nullptr) {
        lv_label_set_text(g_support_label, date_text);
    }
}

void UiStatus_Service(void) {
    if (g_ack_active) {
        if (millis() >= g_ack_end_ms) {
            g_ack_active = false;
            UiStatus_ApplyState(g_ui_state);
        }
        return;
    }

    if (g_ui_state == AppState::IDLE) {
        return;
    }

    const uint32_t now = millis();

    if (g_ui_state == AppState::ALARM_ACTIVE) {
        if (g_last_phrase_ms == 0 || (now - g_last_phrase_ms) >= UI_PHRASE_ROTATE_MS) {
            if (g_last_phrase_ms != 0) {
                g_alarm_phrase_index = static_cast<uint8_t>((g_alarm_phrase_index + 1) % kAlarmPhraseCount);
                ApplyAlarmPhrase();
            }
            g_last_phrase_ms = now;
        }

        if ((now - g_last_anim_ms) >= UI_BREATH_STEP_MS) {
            g_last_anim_ms = now;
            g_breath_step = static_cast<uint8_t>(g_breath_step + UI_BREATH_STEP_AMOUNT);
            const uint8_t wave = TriangleWaveStep(g_breath_step);
            const uint16_t opa_range = static_cast<uint16_t>(UI_MAX_OPACITY - UI_MIN_OPACITY);
            const uint8_t opa = static_cast<uint8_t>(UI_MIN_OPACITY + ((static_cast<uint16_t>(wave) * opa_range) / 255));

            if (g_main_label != nullptr) {
                lv_obj_set_style_text_opa(g_main_label, opa, 0);
            }
        }
        return;
    }

    if (g_last_phrase_ms == 0 || (now - g_last_phrase_ms) >= UI_PHRASE_ROTATE_MS) {
        if (g_last_phrase_ms != 0) {
            g_success_phrase_index = static_cast<uint8_t>((g_success_phrase_index + 1) % kSuccessPhraseCount);
            if (g_main_label != nullptr) {
                lv_label_set_text(g_main_label, kSuccessMainPhrases[g_success_phrase_index]);
            }
            if (g_support_label != nullptr) {
                lv_label_set_text(g_support_label, kSuccessSupportPhrases[g_success_phrase_index]);
            }
        }
        g_last_phrase_ms = now;
    }

    if ((now - g_last_anim_ms) >= UI_BREATH_STEP_MS) {
        g_last_anim_ms = now;

        const uint32_t elapsed = now - g_state_start_ms;
        uint8_t base_opa = UI_MAX_OPACITY;
        if (elapsed < UI_SUCCESS_GLOW_MS) {
            const uint16_t ramp = static_cast<uint16_t>((elapsed * 255U) / UI_SUCCESS_GLOW_MS);
            base_opa = static_cast<uint8_t>(UI_MIN_OPACITY + ((ramp * static_cast<uint16_t>(UI_MAX_OPACITY - UI_MIN_OPACITY)) / 255));
        }

        if (g_main_label != nullptr) {
            lv_obj_set_style_text_opa(g_main_label, base_opa, 0);
        }
    }
}

void UiStatus_ShowAcknowledgement(void) {
    ShowAcknowledgement();
}
