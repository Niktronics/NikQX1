#include <Arduino.h>
#include <esp_sleep.h>
#include <Joystick.h>
#include <DisplayManager.h>
#include <EspNowManager.h>

constexpr gpio_num_t JOYSTICK_BUTTON_GPIO = GPIO_NUM_32;
constexpr uint32_t INACTIVITY_TIMEOUT = 60000;
constexpr int JOYSTICK_ACTIVITY_THRESHOLD = 150;

Joystick joystick;
DisplayManager displayManager(joystick);
EspNowManager espNowManager;

void enterDeepSleep();
bool hasJoystickActivity(int x, int y, int previousX, int previousY, bool pressed, bool previousPressed);

void setup() {
    Serial.begin(115200);
    joystick.init();
    displayManager.init();
    espNowManager.init();
}

void loop() {
    static bool initialized = false;
    static int previousX = 0;
    static int previousY = 0;
    static bool previousPressed = false;
    static uint32_t lastActivityTime = millis();

    joystick.loop();

    const int x = joystick.getX();
    const int y = joystick.getY();
    const bool pressed = joystick.isPressed();

    if (!initialized || hasJoystickActivity(x, y, previousX, previousY, pressed, previousPressed)) {
        lastActivityTime = millis();
        initialized = true;
        previousX = x;
        previousY = y;
        previousPressed = pressed;
    }

    if (millis() - lastActivityTime >= INACTIVITY_TIMEOUT) enterDeepSleep();

    displayManager.loop();
    uint8_t command = displayManager.getCommand();
    espNowManager.sendCommand(command);

    Serial.println("x: " + String(joystick.getX()) + " y: " + String(joystick.getY()) + " pressed: " + String(joystick.isPressed()) + " longPressed: " + String(joystick.isLongPressed()));

    delay(20);
}

void enterDeepSleep() {
    esp_sleep_enable_ext0_wakeup(JOYSTICK_BUTTON_GPIO, 0);
    while (digitalRead(JOYSTICK_BUTTON_GPIO) == LOW) delay(10);
    delay(50);
    esp_deep_sleep_start();
}

bool hasJoystickActivity(int x, int y, int previousX, int previousY, bool pressed, bool previousPressed) {
    return abs(x - previousX) >= JOYSTICK_ACTIVITY_THRESHOLD || abs(y - previousY) >= JOYSTICK_ACTIVITY_THRESHOLD || pressed != previousPressed;
}