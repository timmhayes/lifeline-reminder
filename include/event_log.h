#pragma once

#include <time.h>

#include <Arduino.h>

bool EventLog_Init(void);
bool EventLog_AppendSuccess(const tm& local_time);
String EventLog_ReadAll(void);
bool EventLog_Clear(void);
