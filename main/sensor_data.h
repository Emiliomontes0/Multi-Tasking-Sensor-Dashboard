#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Message types for different sensor data
typedef enum {
    SENSOR_MSG_LIGHT,
    SENSOR_MSG_TEMPERATURE_HUMIDITY,
    SENSOR_MSG_DASHBOARD_UPDATE
} sensor_msg_type_t;

// Light sensor data structure
typedef struct {
    sensor_msg_type_t type;
    int light_level;
    bool valid;
    uint32_t timestamp;
} light_sensor_msg_t;

// Temperature and humidity data structure
typedef struct {
    sensor_msg_type_t type;
    float temperature_celsius;
    float temperature_fahrenheit;
    float humidity;
    bool temperature_valid;
    bool humidity_valid;
    uint32_t timestamp;
} temp_humidity_msg_t;

// Dashboard update message
typedef struct {
    sensor_msg_type_t type;
    light_sensor_msg_t light_data;
    temp_humidity_msg_t temp_humidity_data;
    uint32_t timestamp;
} dashboard_msg_t;

// Queue handles (extern declarations)
extern QueueHandle_t light_queue;
extern QueueHandle_t temp_humidity_queue;
extern QueueHandle_t logger_queue;

// Function prototypes
void sensor_data_init(void);
void clear_screen(void);
void update_dashboard(const dashboard_msg_t *data);
