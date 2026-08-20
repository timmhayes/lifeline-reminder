#include <WiFi.h>
#include <time.h>
#include <lvgl.h>
#include "lv_conf.h"
#include "LVGL_Driver.h"
#include "Display_ST7789.h"
#include "config.h"
#include "app_state.h"
#include "sensor_monitor.h"
#include "led_status.h"
#include "ui_status.h"
#include "event_log.h"
#include "web_dashboard.h"

lv_obj_t * main_label;
lv_obj_t * support_label;
AppState g_last_applied_state = AppState::IDLE;

void setupUI() {
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(COLOR_BACKGROUND), 0);

    main_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(main_label, TIME_LABEL_FONT, 0);
    lv_obj_set_style_text_color(main_label, lv_color_hex(COLOR_TIME_TEXT), 0);
    lv_label_set_text(main_label, "Syncing...");
    lv_obj_align(main_label, LV_ALIGN_CENTER, 0, TIME_LABEL_OFFSET_Y);

    support_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(support_label, DATE_LABEL_FONT, 0);
    lv_obj_set_style_text_color(support_label, lv_color_hex(COLOR_DATE_TEXT), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(support_label, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_label_set_text(support_label, "");
    lv_obj_align(support_label, LV_ALIGN_CENTER, 0, DATE_LABEL_OFFSET_Y);
}

void connectWiFi() {
    const char* ssids[] = { WIFI_SSID, WIFI_BACKUP_SSID };
    const char* passwords[] = { WIFI_PASSWORD, WIFI_BACKUP_PASSWORD };

    while (WiFi.status() != WL_CONNECTED) {
        for (int attempt = 0; attempt < 2 && WiFi.status() != WL_CONNECTED; ++attempt) {
            if (ssids[attempt] == nullptr || ssids[attempt][0] == '\0') {
                continue;
            }

            lv_label_set_text(main_label, attempt == 0 ? "Connecting to WiFi..." : "Trying backup WiFi...");
            Serial.printf("Connecting with %s\n", ssids[attempt]);
            WiFi.begin(ssids[attempt], passwords[attempt]);

            const uint32_t start_time = millis();
            while (WiFi.status() != WL_CONNECTED && (millis() - start_time) < WIFI_CONNECT_ATTEMPT_MS) {
                delay(WIFI_CHECK_INTERVAL);
                Serial.print(".");
            }

            if (WiFi.status() == WL_CONNECTED) {
                break;
            }

            WiFi.disconnect(true);
            delay(WIFI_CHECK_INTERVAL);
        }

        if (WiFi.status() != WL_CONNECTED) {
            lv_label_set_text(main_label, "Retrying WiFi...");
        }
    }

    Serial.println(" Connected!");
}

void syncNetworkTime() {
    configTzTime(NTP_TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);
}

void updateTimeDisplay(const tm& timeinfo) {
    static uint32_t last_time_update = 0;
    
    if (millis() - last_time_update > TIME_UPDATE_INTERVAL) {
        last_time_update = millis();

        char time_str[32];
        strftime(time_str, sizeof(time_str), TIME_FORMAT_STR, &timeinfo);

        char date_str[64];
        strftime(date_str, sizeof(date_str), DATE_FORMAT_STR, &timeinfo);

        UiStatus_UpdateClock(time_str, date_str);
    }
}

void setup() {
    Serial.begin(115200);

    LCD_Init(); 
    Set_Backlight(10);
    Lvgl_Init();

    setupUI();

    connectWiFi();
    syncNetworkTime();

    EventLog_Init();
    AppState_Init();
    SensorMonitor_Init();
    LedStatus_Init();

    UiStatus_Init(main_label, support_label);
    UiStatus_ApplyState(AppState_Get());
    LedStatus_SetState(AppState_Get());

    WebDashboard_Init();
}

void loop() {
    struct tm local_time;
    const bool has_time = getLocalTime(&local_time, GET_LOCAL_TIME_TIMEOUT);

    if (has_time) {
        AppState_ServiceSchedule(local_time);
        updateTimeDisplay(local_time);
    }

    const bool sensor_triggered = SensorMonitor_Poll();
    // EventLog_Clear();  // Clear the log file at the start of each loop iteration
    if (sensor_triggered) {
        if (AppState_Get() == AppState::ALARM_ACTIVE && AppState_TriggerSuccess()) {
            if (has_time) {
                EventLog_AppendSuccess(local_time);
            } else {
                struct tm fallback_time;
                if (getLocalTime(&fallback_time, GET_LOCAL_TIME_TIMEOUT)) {
                    EventLog_AppendSuccess(fallback_time);
                }
            }
        } else if (AppState_IsClickedToday()) {
            UiStatus_ShowAcknowledgement();
        }
    }

    const AppState current_state = AppState_Get();
    if (current_state != g_last_applied_state) {
        UiStatus_ApplyState(current_state);
        LedStatus_SetState(current_state);
        g_last_applied_state = current_state;
    }

    UiStatus_Service();
    LedStatus_Service();
    WebDashboard_Service();
    
    lv_tick_inc(LVGL_TICK_PERIOD); 
    lv_timer_handler();
    delay(LOOP_DELAY);
}