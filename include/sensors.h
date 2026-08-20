#pragma once

#include <stdint.h>

// ============================================================================
// Sensor Data Structure
// ============================================================================
struct SensorData {
    // Temperature and humidity (when sensors are added)
    float temperature;
    float humidity;
    
    // Additional sensor readings can be added here
    // float pressure;
    // float light_level;
    // etc.
};

// ============================================================================
// Sensor Interface Functions
// ============================================================================

/**
 * Initialize all connected sensors
 */
void Sensors_Init(void);

/**
 * Read all connected sensors
 * @param sensor_data Pointer to SensorData structure to fill with readings
 * @return true if read was successful, false otherwise
 */
bool Sensors_Read(SensorData* sensor_data);

/**
 * Get the last read sensor data
 * @return Reference to the last sensor data read
 */
const SensorData* Sensors_GetLastData(void);
