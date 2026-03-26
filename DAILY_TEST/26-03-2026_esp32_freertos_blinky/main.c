#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define LED_PIN GPIO_NUM_2


static const char *TAG = "monitor";


void display(void *arg)
{
    
    while(1)
    {
        vTaskDelay(5000/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "System Alive");
    }
}

void toggle(void *arg)
{
    bool led_state = false;
    while(1)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        led_state = !led_state;
        gpio_set_level(LED_PIN, led_state);
    }
}

void app_main()
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    xTaskCreate(display,"display",2048, NULL, 5, NULL);
    xTaskCreate(toggle,"led_toggle",2048, NULL, 5, NULL);
    
    
}