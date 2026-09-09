#pragma once

#include <cstdint>

class Joystick {
    public:
        void init();
        void loop();
        int getX() const;
        int getY() const;
        bool isPressed() const;
        bool isLongPressed() const;

    private:
        int x = 0, y = 0;
        bool pressed = false;
        bool realPress = false;
        bool longPress = false;
        uint32_t lastDebounceTime = 0;
};