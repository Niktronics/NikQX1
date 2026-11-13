#include "Joystick.h"
#include <Arduino.h>

constexpr int JOYSTICK_X_PIN = 33;
constexpr int JOYSTICK_Y_PIN = 39;
constexpr int JOYSTICK_BUTTON_PIN = 32;

constexpr uint32_t DEBOUNCE_TIME = 250;
constexpr uint32_t LONG_DEBOUNCE_TIME = 1000;
constexpr uint32_t DOUBLE_CLICK_GAP = 400;

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
        const bool wasPressed = realPress;
        realPress = pressed;
        longPress = false;
        lastDebounceTime = now;

        if (realPress) {
            pressStartTime = now;
        } else if (wasPressed) {
            if (now - pressStartTime < LONG_DEBOUNCE_TIME) {
                if (clickCount >= 1 && clickCount < 2 && now - lastClickTime <= DOUBLE_CLICK_GAP) {
                    clickCount++;
                } else {
                    clickCount = 1;
                    doubleClick = false;
                }
                lastClickTime = now;

                if (clickCount == 2) doubleClick = true;
            } else {
                clickCount = 0;
                doubleClick = false;
            }
        }
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

bool Joystick::isDoubleClicked() const {
    return doubleClick;
}
