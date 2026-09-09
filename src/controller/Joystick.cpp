#include "Joystick.h"
#include <Arduino.h>

constexpr int JOYSTICK_X_PIN = 39;
constexpr int JOYSTICK_Y_PIN = 33;
constexpr int JOYSTICK_BUTTON_PIN = 32;

constexpr uint32_t DEBOUNCE_TIME = 250;
constexpr uint32_t LONG_DEBOUNCE_TIME = 1000;

void Joystick::init() {
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
}

void Joystick::loop() {
    x = analogRead(JOYSTICK_X_PIN);
    y = analogRead(JOYSTICK_Y_PIN);
    pressed = digitalRead(JOYSTICK_BUTTON_PIN) == LOW;

    const uint32_t now = millis();

    if (pressed != realPress && now - lastDebounceTime > DEBOUNCE_TIME) {
        realPress = pressed;
        longPress = false;
        lastDebounceTime = now;
    }

    if (realPress && !longPress && now - lastDebounceTime > LONG_DEBOUNCE_TIME) longPress = true;
}

int Joystick::getX() const {
    return x;
}

int Joystick::getY() const {
    return y;
}

bool Joystick::isPressed() const {
    return realPress;
}

bool Joystick::isLongPressed() const {
    return longPress;
}
