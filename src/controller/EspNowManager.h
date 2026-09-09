#pragma once

#include <Arduino.h>
#include <esp_now.h>

class EspNowManager {
    public:
        bool init();
        bool sendMove(uint8_t command);

    private:
        uint8_t peerAddress[ESP_NOW_ETH_ALEN]{};
        bool initialized = false;
};
