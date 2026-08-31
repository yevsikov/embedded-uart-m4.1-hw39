# ESP+STM Learning Project

This is a small educational project for the [Embedded Development course](https://beetroot.academy/courses/online/kurs-embedded-development)



## Scheme
Схема підключення

| Компонент | GPIO ESP32-S3 | GPIO STM32 BlackPill F411CE | Примітка |
| --- | --- | --- | --- |
| UART1 TX | GPIO17 | PA10 / Serial1 RX | з'єднати навхрест |
| UART1 RX | GPIO18 | PA9 / Serial1 TX | з'єднати навхрест |
| Кнопка | GPIO5 | PA0 | внутрішній pull-up, натискання = LOW |
| LED | GPIO4 | LED_BUILTIN | для STM32 LED активний LOW |
| GND | GND | GND | спільна земля обов'язкова |

Примітка: UART працює на 115200, 8N1, а рівні сигналів для обох плат мають бути 3.3 V.
