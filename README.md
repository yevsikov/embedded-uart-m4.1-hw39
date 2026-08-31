# ESP+STM Learning Project

This is a small educational project for the [Embedded Development course](https://beetroot.academy/courses/online/kurs-embedded-development)



## Scheme
```

Схема підключення
Компонент	                    GPIO ESP32-S3	            Примітка
UART1 TX (→ RX на STM32)	    GPIO17	                    до PA3 (USART2_RX) на STM32F401, навхрест
UART1 RX (← TX з STM32)	        GPIO18	                    від PA2 (USART2_TX) на STM32F401, навхрест
Кнопка	                        GPIO5	                    внутрішній pull-up, натискання = LOW
LED (індикатор команди з STM32)	GPIO4	                    через резистор ~330 Ом на GND
GND	                            GND	                        спільний з STM32 — обов’язково
UART0 (консоль)	                вбудований USB	            автоматично, для idf.py monitor — не чіпаємо
```
