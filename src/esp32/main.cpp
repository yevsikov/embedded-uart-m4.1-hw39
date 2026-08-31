//ESP32
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_PORT      UART_NUM_1
#define UART_TX_PIN    17
#define UART_RX_PIN    18
#define UART_BAUD      115200
#define UART_BUF_SIZE  256

// Піни залишаємо типізованими, бо GPIO API ESP-IDF очікує gpio_num_t.
static constexpr gpio_num_t BTN_GPIO = GPIO_NUM_5;
static constexpr gpio_num_t LED_GPIO = GPIO_NUM_4;
// Один байт-команда для обох мікроконтролерів: натиснули кнопку -> надіслали toggle.
#define DEBOUNCE_MS   50
#define CMD_TOGGLE    0x01

static const char *TAG = "UART_LINK";

void uart_link_init(void)
{
    // UART1 працює як лінія зв'язку між ESP32 і STM32.
    uart_config_t uart_config = {
        .baud_rate  = UART_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN,
                                  UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_SIZE, UART_BUF_SIZE, 0, NULL, 0));
    ESP_LOGI(TAG, "UART1 готовий: %d 8N1 на GPIO%d(TX)/GPIO%d(RX)", UART_BAUD, UART_TX_PIN, UART_RX_PIN);
}

void setup(void)
{
    uart_link_init();

    // Локальна кнопка як вхід, локальний LED як вихід.
    gpio_set_direction(BTN_GPIO, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

void loop(void)
{
    static int last_btn = 1;
    static int64_t last_change_us = 0;
    static int led_state = 0;

    while (1) {
        // Дебаунс: реагуємо лише на стабільне натискання кнопки.
        int btn = gpio_get_level(BTN_GPIO);
        if (btn != last_btn) {
            int64_t now_us = esp_timer_get_time();
            if (now_us - last_change_us > DEBOUNCE_MS * 1000) {
                last_change_us = now_us;
                if (btn == 0) {   // LOW означає натискання, бо кнопка підтягнута pull-up'ом.
                    uint8_t cmd = CMD_TOGGLE;
                    uart_write_bytes(UART_PORT, (const char *)&cmd, 1);
                    ESP_LOGI(TAG, "Кнопка натиснута -> надіслано 0x%02X", cmd);
                }
            }
            last_btn = btn;
        }

        // Неблокуюче читання UART: якщо прийшла команда, перемикаємо локальний LED.
        uint8_t rx_byte;
        int len = uart_read_bytes(UART_PORT, &rx_byte, 1, 0);
        if (len > 0 && rx_byte == CMD_TOGGLE) {
            led_state = !led_state;
            gpio_set_level(LED_GPIO, led_state);
            ESP_LOGI(TAG, "Отримано 0x%02X -> LED = %d", rx_byte, led_state);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}