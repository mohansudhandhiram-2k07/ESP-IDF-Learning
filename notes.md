# ESP IDF syntax for reference

- strapping pins - when booting it checks the voltage (0/1) to lock in the mode. after boot behaves as a normal one

- TXD - TRANSMIT DATA, RXD - READ DATA

- JTAG - debugging, use it later.

## GPIO FUNCTIONS
- Header file - "driver/gpio.h"
    - gpio_config()
    - gpio_dump_io_configuration(stdout,(1ULL << 4) | (1ULL << 5))
