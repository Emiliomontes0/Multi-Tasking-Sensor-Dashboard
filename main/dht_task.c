#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "dht_task.h"
#include "sensor_data.h"

#define DHT_PIN GPIO_NUM_25
#define DHT_TIMEOUT_US 1000
#define DHT_SAMPLE_PERIOD_MS 3000

// DHT11 timing constants (microseconds)
#define DHT_START_SIGNAL_LOW_US 18000
#define DHT_START_SIGNAL_HIGH_US 40
#define DHT_RESPONSE_LOW_US 80
#define DHT_RESPONSE_HIGH_US 80
#define DHT_DATA_BIT_LOW_US 50
#define DHT_DATA_BIT_HIGH_0_US 26
#define DHT_DATA_BIT_HIGH_1_US 70

typedef struct {
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
    uint8_t checksum;
} dht11_data_t;

static void delay_us(uint32_t us) {
    uint32_t start = esp_timer_get_time();
    while (esp_timer_get_time() - start < us) {
        // Busy wait
    }
}

static bool dht11_read_data(dht11_data_t *data) {
    uint8_t buffer[5] = {0};
    
    // Send start signal
    gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_PIN, 0);
    delay_us(DHT_START_SIGNAL_LOW_US);
    gpio_set_level(DHT_PIN, 1);
    delay_us(DHT_START_SIGNAL_HIGH_US);
    
    // Switch to input mode with pull-up
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(DHT_PIN, GPIO_PULLUP_ONLY);
    
    // Wait for response (sensor pulls low)
    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(DHT_PIN) == 1) {
        if (esp_timer_get_time() - start_time > 100) {
            printf("[DHT] No response from sensor\n");
            return false;
        }
    }
    
    // Wait for response low to end
    start_time = esp_timer_get_time();
    while (gpio_get_level(DHT_PIN) == 0) {
        if (esp_timer_get_time() - start_time > 100) {
            printf("[DHT] Response low timeout\n");
            return false;
        }
    }
    
    // Wait for response high to end
    start_time = esp_timer_get_time();
    while (gpio_get_level(DHT_PIN) == 1) {
        if (esp_timer_get_time() - start_time > 100) {
            printf("[DHT] Response high timeout\n");
            return false;
        }
    }
    
    // Read 40 bits (5 bytes)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            // Wait for data bit low
            start_time = esp_timer_get_time();
            while (gpio_get_level(DHT_PIN) == 0) {
                if (esp_timer_get_time() - start_time > 100) {
                    printf("[DHT] Data bit low timeout\n");
                    return false;
                }
            }
            
            // Measure high time to determine bit value
            start_time = esp_timer_get_time();
            while (gpio_get_level(DHT_PIN) == 1) {
                if (esp_timer_get_time() - start_time > 100) {
                    printf("[DHT] Data bit high timeout\n");
                    return false;
                }
            }
            
            int64_t high_time = esp_timer_get_time() - start_time;
            if (high_time > 40) { // Threshold between 0 and 1
                buffer[i] |= (1 << (7 - j));
            }
        }
    }
    
    // Verify checksum
    uint8_t checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
    if (checksum != buffer[4]) {
        printf("[DHT] Checksum error: calculated=%d, received=%d\n", checksum, buffer[4]);
        return false;
    }
    
    // Parse data
    data->humidity_int = buffer[0];
    data->humidity_dec = buffer[1];
    data->temperature_int = buffer[2];
    data->temperature_dec = buffer[3];
    data->checksum = buffer[4];
    
    return true;
}

void dht_task(void *pvParameter) {
    // Configure GPIO for DHT11
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DHT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    printf("[DHT Task] Starting DHT11 sensor on GPIO %d\n", DHT_PIN);
    printf("[DHT Task] Waiting 2 seconds for sensor to stabilize...\n");
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    int success_count = 0;
    int total_attempts = 0;
    
    while (1) {
        total_attempts++;
        dht11_data_t data;
        
        if (dht11_read_data(&data)) {
            success_count++;
            float temperature_celsius = (float)data.temperature_int + (float)data.temperature_dec / 10.0f;
            float temperature_fahrenheit = (temperature_celsius * 9.0f / 5.0f) + 32.0f;
            float humidity = (float)data.humidity_int + (float)data.humidity_dec / 10.0f;
            
            // Create temperature/humidity message
            temp_humidity_msg_t temp_humidity_msg = {
                .type = SENSOR_MSG_TEMPERATURE_HUMIDITY,
                .temperature_celsius = temperature_celsius,
                .temperature_fahrenheit = temperature_fahrenheit,
                .humidity = humidity,
                .temperature_valid = true,
                .humidity_valid = true,
                .timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS
            };
            
            // Send data to queue
            if (xQueueSend(temp_humidity_queue, &temp_humidity_msg, pdMS_TO_TICKS(100)) == pdTRUE) {
                printf("[DHT Task] Success! Temperature: %.1f°C (%.1f°F), Humidity: %.1f%% (Success rate: %d/%d)\n", 
                       temperature_celsius, temperature_fahrenheit, humidity, success_count, total_attempts);
            } else {
                printf("[DHT Task] Failed to send data (queue full) (Success rate: %d/%d)\n", 
                       success_count, total_attempts);
            }
        } else {
            // Create invalid data message
            temp_humidity_msg_t temp_humidity_msg = {
                .type = SENSOR_MSG_TEMPERATURE_HUMIDITY,
                .temperature_celsius = 0.0f,
                .temperature_fahrenheit = 0.0f,
                .humidity = 0.0f,
                .temperature_valid = false,
                .humidity_valid = false,
                .timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS
            };
            
            // Send invalid data to queue
            xQueueSend(temp_humidity_queue, &temp_humidity_msg, pdMS_TO_TICKS(100));
            
            printf("[DHT Task] Failed to read DHT11 sensor (Success rate: %d/%d)\n", 
                   success_count, total_attempts);
        }
        
        vTaskDelay(pdMS_TO_TICKS(DHT_SAMPLE_PERIOD_MS));
    }
}

void dht_task_init(void) {
    xTaskCreate(dht_task, "dht_task", 4096, NULL, 5, NULL);
}
