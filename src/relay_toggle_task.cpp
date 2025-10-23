#include <Arduino.h>
#include <relay_toggle_task.h>

#define RELAY1_GPIO 4
#define RELAY2_GPIO 5

void relay_toggle_task(void *pvParameters) {
  Serial.println("Relay toggle task started");

  pinMode(RELAY1_GPIO, OUTPUT);
  pinMode(RELAY2_GPIO, OUTPUT);

  digitalWrite(RELAY1_GPIO, LOW);
  digitalWrite(RELAY2_GPIO, LOW);

  while (true) {
    digitalWrite(RELAY1_GPIO, HIGH);
    digitalWrite(RELAY2_GPIO, LOW);
    Serial.println("Relay 1: ON | Relay 2: OFF");
    vTaskDelay(pdMS_TO_TICKS(2000));

    digitalWrite(RELAY1_GPIO, LOW);
    digitalWrite(RELAY2_GPIO, HIGH);
    Serial.println("Relay 1: OFF | Relay 2: ON");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
