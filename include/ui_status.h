#pragma once

#include <lvgl.h>

#include "app_state.h"

void UiStatus_Init(lv_obj_t* main_label, lv_obj_t* support_label);
void UiStatus_ApplyState(AppState state);
void UiStatus_UpdateClock(const char* time_text, const char* date_text);
void UiStatus_Service(void);
void UiStatus_ShowAcknowledgement(void);
