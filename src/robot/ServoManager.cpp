#include "ServoManager.h"

// min 0x9
// max 0xB

void ServoManager::init() {
    LOG("Inizializzando i servo...");
    Serial1.begin(1000000, SERIAL_8N1, 18, 19);
    st.pSerial = &Serial1;
    pref.begin("SERVO", false);

    if (pref.getBool("initialized", false)) {
        LOG("ServoManager inizializzato!");
        return;
    }


    for (int i = 0; i<=7; i++) {

        int id = i+1;
        String servoName = String(toName(servos[i]));

        while (Serial.read() != 'q') {
            LOG("[Q] per impostare il middle point di " + servoName);
            delay(50);
        }

        st.CalibrationOfs(id);

        LOG("Sposta il servo " + servoName + " ai suoi limiti. [Q] per uscire.");

        int min = st.ReadPos(id);
        int max = min;

        while (Serial.read() != 'q') {

            int pos = st.ReadPos(id);

            if (pos<min) min=pos;
            if (pos>max) max=pos;

            LOG("Pos: " + String(pos) + " Min: " + String(min) + " Max: " + String(max));
            delay(50);

        }

        st.unLockEprom(id);
        st.writeWord(id, 0x9, min);
        st.writeWord(id, 0xB, max);
        st.LockEprom(id);

    }

    pref.putBool("initialized", true);
    LOG("ServoManager inizializzato!");
}

void ServoManager::setTorque(uint8_t id, bool enabled) {
    st.EnableTorque(id, enabled);
}

void ServoManager::setAllTorque(bool enabled) {
    for (int i = 0; i < 8; i++) {
        setTorque(i, enabled);
        delay(2);
    }
}

void ServoManager::stopServo(uint8_t id) {
    st.WritePosEx(id, st.ReadPos(id), 0, 0);
}

void ServoManager::stopAllServos() {
    for (int i = 0; i < 8; i++) {
        st.WritePosEx(i, st.ReadPos(i), 0, 0);
        delay(2);
    }
}

void ServoManager::moveServos(const uint8_t ids[], size_t count, short positions[], int speed, int acc) {
    if (count == 0 || count > MAX_SERVOS) return;

    uint16_t speeds[MAX_SERVOS];
    uint8_t accs[MAX_SERVOS];
    for (size_t i = 0; i < count; i++) {
        speeds[i] = static_cast<uint16_t>(speed);
        accs[i] = static_cast<uint8_t>(acc);
    }

    // SyncWritePosEx accetta u8 ID[] non-const per pigrizia della libreria, ma legge solo
    // gli ID (SCS::syncWrite, SCS.cpp:142). const_cast evita una copia inutile su stack.
    st.SyncWritePosEx(const_cast<uint8_t*>(ids), static_cast<uint8_t>(count), positions, speeds, accs);
}

void ServoManager::moveServos(const ServoID servos[], size_t count, short positions[], int speed, int acc) {
    if (count == 0 || count > MAX_SERVOS) return;

    uint8_t ids[MAX_SERVOS];

    for (size_t i = 0; i < count; i++) {
        ids[i] = static_cast<uint8_t>(servos[i]);
    }

    moveServos(ids, count, positions, speed, acc);
}