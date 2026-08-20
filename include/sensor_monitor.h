#pragma once

void SensorMonitor_Init(void);
bool SensorMonitor_Poll(void);
int SensorMonitor_GetLastRaw(void);
int SensorMonitor_GetLastBaseline(void);
