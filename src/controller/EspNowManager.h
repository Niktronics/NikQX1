#pragma once

#include <Arduino.h>
#include <esp_now.h>

class EspNowManager {
    public:
        bool init();
        void sendCommand(uint8_t command);

    private:
        uint8_t peerAddress[ESP_NOW_ETH_ALEN]{};
        bool initialized = false;
        uint32_t lastMessageTime = 0;
};
