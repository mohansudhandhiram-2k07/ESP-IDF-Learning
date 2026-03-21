#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#define LED_PIN GPIO_NUM_2
#define BTN_PIN GPIO_NUM_4


//static volatile bool led_state = false;
static const char *TAG = "app_1";
//static volatile bool led_prev_state = false;
static QueueHandle_t led_queue;

extern "C" { void app_main(void); }

void IRAM_ATTR button_isr_handler(void* arg)
{
  static uint32_t prev_time = 0;
  uint32_t curr_time = xTaskGetTickCountFromISR();
  if(curr_time - prev_time > (300 / portTICK_PERIOD_MS))
  {
    int cmd = 2;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(led_queue, &cmd, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken)
    {
      portYIELD_FROM_ISR();
    }
  }
  
}
void rx_task(void *arg)
{
  uint8_t data[128];
  memset(data, 0, sizeof(data));
  int index = 0;
  while(1)
  {
    uint8_t byte;
    int length = uart_read_bytes(UART_NUM_0,&byte,1,10 / portTICK_PERIOD_MS);
    if(length > 0)
    {
      if(byte == '\r' || byte == '\n')
      {
        data[index] = '\0';
        
        if(index > 0)
        {
          if(strncmp((char *)data, "OFF", 3) == 0 || data[0] == '0')
          {
            ESP_LOGI(TAG,"CMD: OFF\n");
            int val = 0;
            xQueueSend(led_queue,&val,0);
          }
          else if(strncmp((char *)data, "ON", 2) == 0 || data[0] == '1')
          {
            ESP_LOGI(TAG,"CMD: ON\n");
            int val = 1;
            xQueueSend(led_queue,&val,0);
          }
          else
          {
            ESP_LOGE(TAG,"UNKNOWN CMD\n");
          }
          index = 0;
          memset(data,0,sizeof(data));
        }
      }
      else
      {
        data[index++] = byte;
      }
      
      
      
    }
  }
}


void app_main(void) 
{
  led_queue = xQueueCreate(10,sizeof(int));

  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
  
  gpio_reset_pin(BTN_PIN);
  gpio_set_direction(BTN_PIN, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BTN_PIN, GPIO_PULLUP_ONLY); 
  gpio_set_intr_type(BTN_PIN, GPIO_INTR_NEGEDGE); 
  
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BTN_PIN, button_isr_handler, NULL);
  
  uart_config_t uart_config = 
  {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };
  uart_param_config(UART_NUM_0,&uart_config);
  uart_driver_install(UART_NUM_0, 1024*2, 0, 0, NULL, 0);
  xTaskCreate(rx_task, "uart_task", 4096, NULL, 10, NULL);
  
  int received_val;
  bool ledstate = false;

  while(1) 
  {
    if(xQueueReceive(led_queue,&received_val,portMAX_DELAY))
    {
      if(received_val == 2)
      {
        ledstate = !ledstate;
      }
      else
      {
        ledstate = received_val;
      }
      gpio_set_level(LED_PIN,ledstate);
      ESP_LOGI(TAG,"ESP Received value: %d",received_val);
    }
    
  }
}