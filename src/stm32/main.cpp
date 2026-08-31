// STM32

#include <Arduino.h>

static constexpr uint8_t BUTTON_PIN = PA0;
static constexpr uint8_t LED_PIN = LED_BUILTIN;
static constexpr uint32_t UART_BAUD = 115200;
// Той самий байт, що й на ESP32, щоб обидві сторони розуміли одну команду.
static constexpr uint8_t CMD_TOGGLE = 0x01;
static constexpr uint32_t DEBOUNCE_MS = 50;

void setup() {
  // Serial1 — апаратний UART STM32 для зв'язку з ESP32.
  Serial1.begin(UART_BAUD);

  // Кнопка з підтяжкою до живлення, LED працює як активний LOW на BlackPill.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  static int lastButtonState = HIGH;
  static uint32_t lastChangeMs = 0;
  static bool ledState = false;

  // Дивимось на кнопку локально, без зайвого мерехтіння від дребезгу контактів.
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    uint32_t nowMs = millis();
    if (nowMs - lastChangeMs > DEBOUNCE_MS) {
      lastChangeMs = nowMs;
      if (buttonState == LOW) {
        // Надсилаємо одну байтову команду на ESP32.
        Serial1.write(CMD_TOGGLE);
      }
    }
    lastButtonState = buttonState;
  }

  // Якщо ESP32 прислала команду, перемикаємо наш LED.
  while (Serial1.available() > 0) {
    uint8_t rxByte = static_cast<uint8_t>(Serial1.read());
    if (rxByte == CMD_TOGGLE) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? LOW : HIGH);
    }
  }

  delay(10);
}