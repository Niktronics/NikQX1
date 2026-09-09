#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <ServoManager.h>
#include <MotionManager.h>
#include <Logger.h>

ServoManager servoManager;
MotionManager motionManager(servoManager);

using LEG = MotionManager::LEG;
using MOVE = MotionManager::MOVE;
using FRAME = MotionManager::FRAME;

void handleSerialCommand();
void onEspNowDataReceived(const uint8_t *, const uint8_t *data, int dataLength);
void handleReceivedMove();
bool initEspNowReceiver();

volatile uint8_t receivedMove = 0;
volatile bool newMoveAvailable = false;

void setup() {
    Serial.begin(115200);
    servoManager.init();
    initEspNowReceiver();

    // motionManager.perform(MOVE::PUSHUPS);
    // delay(1500);
    // servoManager.setAllTorque(false);
}

void loop() {
    // motionManager.debugPower(LEG::FR, true);
    // motionManager.debugPower(true);
    // motionManager.debugFK(LEG::FR, false);
    // motionManager.debugFK();

    handleSerialCommand();
    handleReceivedMove();

    motionManager.loop();

    delay(10);
}

// 1=STANDING 2=LIE_DOWN 3=WAVE 4=OCTOPUS 5=PURRING 6=SWIMMING
// 7=PUSHUPS 8=WALKING_F 9=WALKING_B 10=ROTATION_CW 11=ROTATION_ACW
void handleSerialCommand() {
    static char buffer[16];
    static uint8_t index = 0;

    while (Serial.available() > 0) {
        char c = (char) Serial.read();

        if (c == '\n' || c == '\r') {
            buffer[index] = '\0';

            if (index > 0) {
                int cmd = atoi(buffer);

                if (cmd >= 1 && cmd <= 11) {
                    LOGF("Eseguo comando %d", cmd);
                    motionManager.perform(static_cast<MOVE>(cmd));
                    LOG("Comando eseguito");
                } else {
                    LOGF("Comando non valido: %s (usare 1-11)", buffer);
                }
            }

            index = 0;
        } else if (index < sizeof(buffer) - 1 && isDigit(c)) {
            buffer[index++] = c;
        }
    }
}

void onEspNowDataReceived(const uint8_t *, const uint8_t *data, int dataLength) {
    if (data == nullptr || dataLength != sizeof(uint8_t) || *data > 11) return;
    receivedMove = *data;
    newMoveAvailable = true;
}

void handleReceivedMove() {
    if (!newMoveAvailable) return;
    const uint8_t move = receivedMove;
    newMoveAvailable = false;
    LOGF("Comando ESP-NOW ricevuto: %u", move);
    motionManager.perform(static_cast<MOVE>(move));
}

bool initEspNowReceiver() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        LOG("Errore inizializzazione espnow");
        return false;
    }

    esp_now_register_recv_cb(onEspNowDataReceived);
    LOGF("mac locale:: %s", WiFi.macAddress().c_str());
    return true;
}
