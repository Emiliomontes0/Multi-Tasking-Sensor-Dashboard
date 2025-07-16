#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int light_level;
    float temperature_celsius;
    float temperature_fahrenheit;
    float humidity;
    bool light_valid;
    bool temperature_valid;
    bool humidity_valid;
} sensor_data_t;

// Global sensor data structure
extern sensor_data_t sensor_data;

// Function to update the dashboard display
void update_dashboard(void);

// Function to clear the screen (platform dependent)
void clear_screen(void);
