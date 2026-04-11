# ESP-IDF-Learning

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-00A86B?style=for-the-badge&logo=freertos&logoColor=white)
![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.x-blue?style=for-the-badge)


> No Arduino. No HAL shortcuts. Raw ESP-IDF from the ground up, one peripheral at a time.

---

## What This Is

This is a structured, ground up learning repository for the **ESP-IDF native framework** on ESP32. Every module is written without the Arduino abstraction layer  direct driver APIs, proper FreeRTOS task architecture, and real interrupt handling.

Each folder is a standalone, buildable ESP-IDF project covering one concept. The goal is to build a deep understanding of the ESP32 peripheral model before applying it to production firmware like [CogniLog](https://github.com/mohansudhandhiram-2k07/CogniLog).

---

## Modules

| # | Module | Concepts Covered |
|---|--------|-----------------|
| 01 | `26-03-2026_esp32_freertos_blinky` | FreeRTOS tasks, `xTaskCreate`, `vTaskDelay`, GPIO output |
| 02 | `28-03-2026_ISR_Queue` | GPIO interrupt, ISR-safe queue (`xQueueSendFromISR`), debounce |
| 03 | `esp32-uart-gpio-interrupt-queue-LED` | UART RX task, GPIO ISR, FreeRTOS queue as unified command bus |
| 04 | `esp32-I2C-comms` | I2C master init, device addressing, read/write transactions |

---

## Module Breakdown

---

### 01 — FreeRTOS Blinky
**`26-03-2026_esp32_freertos_blinky/`**

The first real ESP-IDF project — no `delay()`, no Arduino loop. Two independent FreeRTOS tasks running concurrently on the ESP32.

```c
xTaskCreate(display, "display", 2048, NULL, 5, NULL);  // Logs "System Alive" every 5s
xTaskCreate(toggle,  "led_toggle", 2048, NULL, 5, NULL); // Toggles LED every 500ms
```

**What it demonstrates:**
- `xTaskCreate()` — task handle, stack size, priority
- `vTaskDelay()` with `portTICK_PERIOD_MS` for portable timing
- `gpio_reset_pin()` + `gpio_set_direction()` + `gpio_set_level()` — the correct ESP-IDF GPIO init sequence
- Two tasks at equal priority, cooperatively scheduled by FreeRTOS

---

### 02 — GPIO ISR + Queue
**`28-03-2026_ISR_Queue/`**

Moves beyond polling. A hardware button triggers a GPIO interrupt, which safely signals a FreeRTOS queue from ISR context.

**What it demonstrates:**
- `GPIO_INTR_NEGEDGE` — interrupt fires on button press (falling edge)
- `gpio_install_isr_service()` + `gpio_isr_handler_add()` — per-pin ISR registration
- `xQueueSendFromISR()` + `portYIELD_FROM_ISR()` — the correct pattern for ISR-to-task communication
- Software debounce using `xTaskGetTickCountFromISR()` — 300 ms threshold

**The debounce logic:**
```c
void IRAM_ATTR button_isr_handler(void* arg) {
    static uint32_t prev_time = 0;
    uint32_t curr_time = xTaskGetTickCountFromISR();
    if (curr_time - prev_time > (300 / portTICK_PERIOD_MS)) {
        // safe to send
        xQueueSendFromISR(led_queue, &cmd, &xHigherPriorityTaskWoken);
        prev_time = curr_time;
    }
}
```

> `IRAM_ATTR` places the ISR in internal RAM — required on ESP32 so the handler doesn't fault if flash cache is disabled during a write.

---

### 03 — UART + GPIO ISR + FreeRTOS Queue (Unified Command Bus)
**`esp32-uart-gpio-interrupt-queue-LED/`**

The most complete module in the repo. Two completely independent input sources a **physical button** (hardware interrupt) and a **UART serial terminal** (software task) — both funnel commands into a single FreeRTOS queue consumed by one LED controller.

```
[Button Press]  →  GPIO ISR  →  xQueueSendFromISR()  ─┐
                                                        ├─→  led_queue  →  app_main() → gpio_set_level()
[UART "ON/OFF"] →  rx_task   →  xQueueSend()         ─┘
```

**UART Configuration:**
```c
uart_config_t uart_config = {
    .baud_rate  = 115200,
    .data_bits  = UART_DATA_8_BITS,
    .parity     = UART_PARITY_DISABLE,
    .stop_bits  = UART_STOP_BITS_1,
    .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};
uart_param_config(UART_NUM_0, &uart_config);
uart_driver_install(UART_NUM_0, 1024 * 2, 0, 0, NULL, 0);
```

**UART Command Parser** — `rx_task` reads byte-by-byte at 115200 baud, assembles into a line buffer, then matches commands:

| Command | Action |
|---------|--------|
| `ON` or `1` + Enter | LED ON |
| `OFF` or `0` + Enter | LED OFF |
| Button press | Toggle LED |
| Unknown string | `ESP_LOGE` error log |

**Queue command encoding:**
- `val = 0` → LED OFF
- `val = 1` → LED ON
- `val = 2` → Toggle (from ISR)

**What it demonstrates:**
- Multi-source input architecture using a queue as a thread-safe command bus
- `uart_read_bytes()` with timeout for non-blocking byte reads
- `strncmp()` based command parsing in embedded C++
- `extern "C" { void app_main(void); }` — C linkage in a `.cpp` file
- `xTaskCreate` with `4096` stack for UART task (larger than blinky — string ops need stack)
- `portMAX_DELAY` on `xQueueReceive` — blocks indefinitely until a command arrives

---

### 04 — I2C Master Communication
**`esp32-I2C-comms/`**

ESP-IDF native I2C master driver  no Wire library, no Arduino abstractions.

> *(Details to be added — module in progress)*

---

## Hardware Setup

| Pin | Role | Module |
|-----|------|--------|
| GPIO 2 | LED (onboard) | All modules |
| GPIO 4 | Button input (PULLUP, NEGEDGE ISR) | 02, 03 |
| UART0 TX/RX | Serial terminal @ 115200 baud | 03 |
| I2C SDA/SCL | I2C master bus | 04 |

---

## Key ESP-IDF Patterns Demonstrated

| Pattern | Where |
|---------|-------|
| Correct GPIO init sequence (`reset` → `direction` → `level`) | All GPIO modules |
| `IRAM_ATTR` on ISR functions | Module 02, 03 |
| ISR-safe queue send + `portYIELD_FROM_ISR` | Module 02, 03 |
| Tick-based software debounce | Module 02, 03 |
| Byte-by-byte UART line assembly | Module 03 |
| Queue as unified multi-source command bus | Module 03 |
| `extern "C"` for `app_main` in `.cpp` files | Module 03 |
| `portMAX_DELAY` for blocking queue receive | Module 03 |

---

## Build & Flash (Any Module)

```bash
# Navigate to any module folder
cd 26-03-2026_esp32_freertos_blinky

# Set target
idf.py set-target esp32

# Build
idf.py build

# Flash + monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

To test Module 03 UART commands, open the IDF monitor and type `ON`, `OFF`, or press the button on GPIO 4.

---

## Notes File

`DAILY_TEST/` contains raw syntax notes and API reference snippets accumulated during learning GPIO function signatures, strapping pin behaviour, JTAG notes, and more. It's a living reference, not polished code.

---

## Repo Purpose

This is the **foundation layer** for all future ESP32 firmware. Every pattern here — ISR-safe queues, UART parsers, multi-task architecture is directly reused in [CogniLog](https://github.com/mohansudhandhiram-2k07/CogniLog).

---
