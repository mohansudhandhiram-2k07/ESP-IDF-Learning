# ESP IDF syntax for reference

- strapping pins - when booting it checks the voltage (0/1) to lock in the mode. after boot behaves as a normal one

- TXD - TRANSMIT DATA, RXD - READ DATA

- JTAG - debugging, use it later.
```c
## GPIO FUNCTIONS
- Header file - "driver/gpio.h"
- CMakeLists - "REQUIRES esp_driver_gpio"
    - gpio_config()
    - ``esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)`` -> ESP_OK | ESP_ERR_ARG
    - ``esp_err_t gpio_reset_pin(gpio_num_t gpio_num)`` 
    - `esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)`
    - `esp_err_t gpio_intr_enable(gpio_num_t gpio_num)`
    - `esp_err_t gpio_intr_disable(gpio_num_t gpio_num)`
    - `esp_err_t gpio_st_level(gpio_num_t gpio_num, uint32_t level)`
    - *pGPIOConfig is a pointer to config struct
    - gpio_dump_io_configuration(stdout,(1ULL << 4) | (1ULL << 5))

