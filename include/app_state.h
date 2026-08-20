#pragma once

#include <time.h>

// Core runtime states for the daily safety check-in flow.
enum class AppState {
    IDLE = 0,
    ALARM_ACTIVE,
    SUCCESS,
};

void AppState_Init(void);
AppState AppState_Get(void);
bool AppState_Set(AppState new_state);
bool AppState_TriggerSuccess(void);
bool AppState_IsClickedToday(void);
void AppState_ServiceSchedule(const tm& local_time);
