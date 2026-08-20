#include "sensors.h"
#include <cstddef>

// ============================================================================
// Sensor Data Storage
// ============================================================================
static SensorData last_sensor_data = {
    .temperature = 0.0f,
    .humidity = 0.0f,
};

// ============================================================================
// Sensor Interface Implementation
// ============================================================================

void Sensors_Init(void) {
    // TODO: Initialize individual sensors here
    // - I2C/SPI bus initialization if needed
    // - Sensor-specific setup (calibration, etc.)
    // Example:
    // - DHT sensor initialization
    // - BMP sensor initialization
    // - Light sensor initialization
    // - etc.
}

bool Sensors_Read(SensorData* sensor_data) {
    if (sensor_data == NULL) {
        return false;
    }

    // TODO: Read from actual sensors and populate sensor_data
    // Example (when sensors are added):
    // if (!DHT_Read(&sensor_data->temperature, &sensor_data->humidity)) {
    //     return false;
    // }
    
    // For now, return placeholder values
    sensor_data->temperature = 0.0f;
    sensor_data->humidity = 0.0f;
    
    // Cache the last read
    last_sensor_data = *sensor_data;
    
    return true;
}

const SensorData* Sensors_GetLastData(void) {
    return &last_sensor_data;
}
