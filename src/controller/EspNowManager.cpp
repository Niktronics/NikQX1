#include "EspNowManager.h"
#include <WiFi.h>

constexpr uint8_t NO_COMMAND = 255;
constexpr uint32_t SEND_INTERVAL = 100;
constexpr uint8_t BROADCAST_ADDRESS[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

bool EspNowManager::init() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("errore espnow");
        return false;
    }

    memcpy(peerAddress, BROADCAST_ADDRESS, ESP_NOW_ETH_ALEN);

    esp_now_peer_info_t peer{};
    memcpy(peer.peer_addr, peerAddress, ESP_NOW_ETH_ALEN);
    peer.channel = 0;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;

    if (esp_now_add_peer(&peer) != ESP_OK) {
        Serial.println("errore peer espnow");
        esp_now_deinit();
        return false;
    }

    initialized = true;
    return true;
}

void EspNowManager::sendCommand(uint8_t command) {
    if (!initialized || command == NO_COMMAND || millis() - lastMessageTime < SEND_INTERVAL) return;
    esp_now_send(peerAddress, &command, sizeof(command));
    lastMessageTime = millis();
}
