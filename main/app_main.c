#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adc_task.h"
#include "dht_task.h"

void app_main(void)
{
    adc_task_init();
    dht_task_init();
}
