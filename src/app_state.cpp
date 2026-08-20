#include "app_state.h"

#include "config.h"

namespace {
struct AppContext {
    AppState current_state = AppState::IDLE;
    bool clicked_today = false;
    int last_reset_day = -1;
    int last_alarm_day = -1;
};

AppContext g_app;

bool IsPastOrAtTime(const tm& local_time, int hour, int minute) {
    if (local_time.tm_hour > hour) {
        return true;
    }
    if (local_time.tm_hour == hour && local_time.tm_min >= minute) {
        return true;
    }
    return false;
}
}  // namespace

void AppState_Init(void) {
    g_app.current_state = AppState::IDLE;
    g_app.clicked_today = false;
}

AppState AppState_Get(void) {
    return g_app.current_state;
}

bool AppState_Set(AppState new_state) {
    if (g_app.current_state == new_state) {
        return false;
    }

    g_app.current_state = new_state;
    return true;
}

bool AppState_TriggerSuccess(void) {
    if (g_app.current_state != AppState::ALARM_ACTIVE || g_app.clicked_today) {
        return false;
    }

    g_app.clicked_today = true;
    return AppState_Set(AppState::SUCCESS);
}

bool AppState_IsClickedToday(void) {
    return g_app.clicked_today;
}

void AppState_ServiceSchedule(const tm& local_time) {
    const int day_of_year = local_time.tm_yday;

    if (local_time.tm_hour == RESET_HOUR &&
        local_time.tm_min == RESET_MINUTE &&
        g_app.last_reset_day != day_of_year) {
        g_app.clicked_today = false;
        g_app.last_reset_day = day_of_year;
        AppState_Set(AppState::IDLE);
    }

    if (IsPastOrAtTime(local_time, ALARM_HOUR, ALARM_MINUTE) &&
        g_app.last_alarm_day != day_of_year) {
        g_app.last_alarm_day = day_of_year;
        if (!g_app.clicked_today) {
            AppState_Set(AppState::ALARM_ACTIVE);
        }
    }
}
