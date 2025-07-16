#include <stdio.h>
#include "sensor_data.h"

// Global sensor data structure
sensor_data_t sensor_data = {0};

void clear_screen(void) {
    // Clear screen using ANSI escape codes
    printf("\033[2J\033[H");
}

void update_dashboard(void) {
    // Clear screen and show dashboard
    clear_screen();
    
    printf("---------------------------\n");
    printf("[ Sensor Dashboard ]\n");
    
    if (sensor_data.light_valid) {
        printf("Light Level (ADC): %d\n", sensor_data.light_level);
    } else {
        printf("Light Level (ADC): --\n");
    }
    
    if (sensor_data.temperature_valid) {
        printf("Temperature: %.1f°C / %.1f°F\n", 
               sensor_data.temperature_celsius, 
               sensor_data.temperature_fahrenheit);
    } else {
        printf("Temperature: --°C / --°F\n");
    }
    
    if (sensor_data.humidity_valid) {
        printf("Humidity: %.1f%%\n", sensor_data.humidity);
    } else {
        printf("Humidity: --%%\n");
    }
    
    printf("---------------------------\n");
}
