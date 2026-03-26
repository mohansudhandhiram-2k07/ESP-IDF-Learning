# ESP IDF syntax for reference

- strapping pins - when booting it checks the voltage (0/1) to lock in the mode. after boot behaves as a normal one

- TXD - TRANSMIT DATA, RXD - READ DATA

- JTAG - debugging, use it later.
#```c
## GPIO FUNCTIONS
- Header file - "driver/gpio.h"
- CMakeLists - "REQUIRES esp_driver_gpio"
    - gpio_config()
    - esp_err_t gpio_config(const gpio_config_t *pGPIOConfig) -> ESP_OK | ESP_ERR_ARG
    - esp_err_t gpio_reset_pin(gpio_num_t gpio_num)
    - `esp_err_t gpio_install_isr_service(int intr_alloc_flags)`
    - `esp_err_t gpio_uninstall_isr_service(void)`
    - `esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)`
    - `esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)`
    - `esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)`
    - `esp_err_t gpio_intr_enable(gpio_num_t gpio_num)`
    - `esp_err_t gpio_intr_disable(gpio_num_t gpio_num)`
    - `esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)`
    - `esp_err_t gpio_get_level(gpio_num_t gpio_num)`
    - `esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)`
    - `esp_err_t gpio_input_enable(gpio_num_t gpio_num)`
    - `esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_mode_pull_up_t pull)`
    - `esp_err_t gpio_pullup_en(gpio_num_t gpio_num)` or `gpio_pullup_dis(gpio_num_t gpio_num)` --> cannot use/change to pull down
    - `esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)` or `gpio_pulldown_dis(gpio_num_t gpio_num)`
    - `esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)` -->  Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
    - `esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num, )`
    - `esp_err_t gpio_od_en(gpio_num_t gpio_num)`
    - `esp_err_t gpio_od_dis(gpio_num_t gpio_num)`
    - `esp_err_t gpio_isr_register(void (*fun)(void *), void *arg, int intr_alloc_falgs, gpio_isr_handle_t *handle)`
    - ``
    - *pGPIOConfig is a pointer to config struct
    - gpio_dump_io_configuration(stdout,(1ULL << 4) | (1ULL << 5))

