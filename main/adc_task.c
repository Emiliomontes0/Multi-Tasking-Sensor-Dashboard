#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "adc_task.h"
#include "sensor_data.h"

void adc_task(void *pvParameter) {
    // Configure ADC
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // Configure ADC channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &config));

    printf("[ADC Task] Starting ADC task for light sensor\n");

    while (1) {
        int raw_value;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_4, &raw_value));
        
        // Create light sensor message
        light_sensor_msg_t light_msg = {
            .type = SENSOR_MSG_LIGHT,
            .light_level = raw_value,
            .valid = true,
            .timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS
        };
        
        // Send data to queue
        if (xQueueSend(light_queue, &light_msg, pdMS_TO_TICKS(100)) == pdTRUE) {
            printf("[ADC Task] Sent light data: %d\n", raw_value);
        } else {
            printf("[ADC Task] Failed to send light data (queue full)\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void adc_task_init(void) {
    xTaskCreate(adc_task, "adc_task", 2048, NULL, 5, NULL);
}
