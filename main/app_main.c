#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adc_task.h"
#include "dht_task.h"
#include "sensor_data.h"

void app_main(void)
{
    // Initialize sensor data system (queues and logger task)
    sensor_data_init();
    
    // Start sensor tasks
    adc_task_init();
    dht_task_init();
    
    printf("[Main] All tasks initialized successfully\n");
}
