#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_4

static const QueueHandle_t queue;

void app_main(void)
{
    queue = xQueueCreate(10, sizeof(uint32_t));
    esp_err_t esp_set_level(gpio_num_t LED_PIN, 1);
    esp_err_t esp_set_level(gpio_num_t BUTTON_PIN, 0);
    
}