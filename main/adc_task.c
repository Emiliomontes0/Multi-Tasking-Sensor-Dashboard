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

    while (1) {
        int raw_value;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_4, &raw_value));
        
        // Update shared sensor data
        sensor_data.light_level = raw_value;
        sensor_data.light_valid = true;
        
        // Update dashboard
        update_dashboard();
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void adc_task_init(void) {
    xTaskCreate(adc_task, "adc_task", 2048, NULL, 5, NULL);
}
