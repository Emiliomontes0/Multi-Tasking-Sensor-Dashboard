#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sensor_data.h"

// Queue handles
QueueHandle_t light_queue;
QueueHandle_t temp_humidity_queue;
QueueHandle_t logger_queue;

// Logger task function prototype
static void logger_task(void *pvParameter);

void sensor_data_init(void) {
    // Create queues
    light_queue = xQueueCreate(5, sizeof(light_sensor_msg_t));
    temp_humidity_queue = xQueueCreate(5, sizeof(temp_humidity_msg_t));
    logger_queue = xQueueCreate(10, sizeof(dashboard_msg_t));
    
    if (light_queue == NULL || temp_humidity_queue == NULL || logger_queue == NULL) {
        printf("[ERROR] Failed to create queues\n");
        return;
    }
    
    // Create logger task
    xTaskCreate(logger_task, "logger_task", 4096, NULL, 3, NULL);
    
    printf("[Sensor Data] Queues and logger task initialized\n");
}

void clear_screen(void) {
    // Clear screen using ANSI escape codes
    printf("\033[2J\033[H");
}

void update_dashboard(const dashboard_msg_t *data) {
    // Clear screen and show dashboard
    clear_screen();
    
    printf("---------------------------\n");
    printf("[ Sensor Dashboard ]\n");
    printf("Timestamp: %lu ms\n", data->timestamp);
    
    if (data->light_data.valid) {
        printf("Light Level (ADC): %d\n", data->light_data.light_level);
    } else {
        printf("Light Level (ADC): --\n");
    }
    
    if (data->temp_humidity_data.temperature_valid) {
        printf("Temperature: %.1f°C / %.1f°F\n", 
               data->temp_humidity_data.temperature_celsius, 
               data->temp_humidity_data.temperature_fahrenheit);
    } else {
        printf("Temperature: --°C / --°F\n");
    }
    
    if (data->temp_humidity_data.humidity_valid) {
        printf("Humidity: %.1f%%\n", data->temp_humidity_data.humidity);
    } else {
        printf("Humidity: --%%\n");
    }
    
    printf("---------------------------\n");
}

static void logger_task(void *pvParameter) {
    dashboard_msg_t dashboard_data = {0};
    light_sensor_msg_t light_msg;
    temp_humidity_msg_t temp_humidity_msg;
    
    // Initialize dashboard data
    dashboard_data.type = SENSOR_MSG_DASHBOARD_UPDATE;
    dashboard_data.light_data.valid = false;
    dashboard_data.temp_humidity_data.temperature_valid = false;
    dashboard_data.temp_humidity_data.humidity_valid = false;
    
    printf("[Logger Task] Starting logger task\n");
    
    while (1) {
        // Check for new light sensor data
        if (xQueueReceive(light_queue, &light_msg, 0) == pdTRUE) {
            dashboard_data.light_data = light_msg;
            dashboard_data.timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
            printf("[Logger] Received light data: %d (valid: %s)\n", 
                   light_msg.light_level, light_msg.valid ? "true" : "false");
        }
        
        // Check for new temperature/humidity data
        if (xQueueReceive(temp_humidity_queue, &temp_humidity_msg, 0) == pdTRUE) {
            dashboard_data.temp_humidity_data = temp_humidity_msg;
            dashboard_data.timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
            printf("[Logger] Received temp/humidity data: %.1f°C, %.1f%%\n", 
                   temp_humidity_msg.temperature_celsius, temp_humidity_msg.humidity);
        }
        
        // Update dashboard if we have any valid data
        if (dashboard_data.light_data.valid || 
            dashboard_data.temp_humidity_data.temperature_valid || 
            dashboard_data.temp_humidity_data.humidity_valid) {
            update_dashboard(&dashboard_data);
        }
        
        // Small delay to prevent excessive CPU usage
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
