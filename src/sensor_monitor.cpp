#include "sensor_monitor.h"

#include <Arduino.h>

#include "config.h"

namespace {
int g_samples[SENSOR_BASELINE_WINDOW] = {0};
size_t g_sample_count = 0;
size_t g_next_index = 0;
long g_sum = 0;
uint32_t g_last_sample_ms = 0;
uint32_t g_last_trigger_ms = 0;
uint32_t g_total_samples = 0;
int g_last_raw = 0;
int g_last_baseline = 0;

void UpdateWindow(int sample) {
    if (g_sample_count < SENSOR_BASELINE_WINDOW) {
        g_samples[g_next_index] = sample;
        g_sum += sample;
        g_next_index = (g_next_index + 1) % SENSOR_BASELINE_WINDOW;
        g_sample_count++;
        return;
    }

    g_sum -= g_samples[g_next_index];
    g_samples[g_next_index] = sample;
    g_sum += sample;
    g_next_index = (g_next_index + 1) % SENSOR_BASELINE_WINDOW;
}
}  // namespace

void SensorMonitor_Init(void) {
    pinMode(PHOTORESISTOR_PIN, INPUT);
}

bool SensorMonitor_Poll(void) {
    const uint32_t now = millis();
    if ((now - g_last_sample_ms) < SENSOR_SAMPLE_INTERVAL_MS) {
        return false;
    }
    g_last_sample_ms = now;

    const int raw = analogRead(PHOTORESISTOR_PIN);
    // Serial.printf("SensorMonitor_Poll: raw=%d, baseline=%d\n", raw, g_last_baseline);
    g_last_raw = raw;
    // Serial.printf("SensorMonitor_Poll: raw=%d, baseline=%d, delta=%d\n", raw, g_last_baseline, raw - g_last_baseline);

    if (g_sample_count == 0) {
        UpdateWindow(raw);
        g_last_baseline = raw;
        g_total_samples++;
        return false;
    }

    g_last_baseline = static_cast<int>(g_sum / static_cast<long>(g_sample_count));
    const int delta = raw - g_last_baseline;

    const bool warmup_done = g_total_samples >= SENSOR_WARMUP_SAMPLES;
    const bool debounce_done = (now - g_last_trigger_ms) >= SENSOR_DEBOUNCE_MS;
    const bool above_floor = raw >= SENSOR_MIN_ABSOLUTE;
    const bool over_threshold = delta >= SENSOR_TRIGGER_DELTA;

    bool triggered = false;
    if (warmup_done && debounce_done && above_floor && over_threshold) {
        g_last_trigger_ms = now;
        triggered = true;
    }

    UpdateWindow(raw);
    g_total_samples++;

    return triggered;
}

int SensorMonitor_GetLastRaw(void) {
    return g_last_raw;
}

int SensorMonitor_GetLastBaseline(void) {
    return g_last_baseline;
}
