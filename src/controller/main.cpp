#include <Arduino.h>
#include <Joystick.h>
#include <DisplayManager.h>
#include <EspNowManager.h>

Joystick joystick;
DisplayManager displayManager(joystick);
EspNowManager espNowManager;

void sendCommand(uint8_t command) {
    constexpr uint32_t SEND_INTERVAL = 100;
    static uint32_t lastMessageTime = 0;
    static uint8_t lastCommand = 255;

    if (command == 255 || millis() - lastMessageTime < SEND_INTERVAL) return;
    const bool sent = espNowManager.sendMove(command);

    if (command != lastCommand || !sent) {
        Serial.printf("Comando joystick %u: %s\n", command, sent ? "inviato" : "INVIO FALLITO");
        lastCommand = command;
    }

    lastMessageTime = millis();
}

void setup() {
    Serial.begin(115200);
    joystick.init();
    displayManager.init();
    espNowManager.init();
}

void loop() {
    joystick.loop();
    const int x = joystick.getX();
    const int y = joystick.getY();
    displayManager.loop();
    uint8_t command = displayManager.getCommand();
    if (command != 255) sendCommand(command);

    Serial.println("x: " + String(x) + " y: " + String(y) + " pressed: " + String(joystick.isPressed()) + " longPressed: " + String(joystick.isLongPressed()));

    delay(20);
}
