#pragma once

#include <SMS_STS.h>
#include <Preferences.h>
#include <Logger.h>

class ServoManager {
public:
    enum class ServoID : uint8_t {
        FRONT_RIGHT_LOWER = 1,
        FRONT_RIGHT_UPPER,

        FRONT_LEFT_LOWER,
        FRONT_LEFT_UPPER,

        REAR_RIGHT_LOWER,
        REAR_RIGHT_UPPER,

        REAR_LEFT_LOWER,
        REAR_LEFT_UPPER
    };

    constexpr const char* toName(ServoID servo) {
        switch (servo) {
            case ServoID::FRONT_RIGHT_UPPER: return "FRONT_RIGHT_UPPER";
            case ServoID::FRONT_RIGHT_LOWER: return "FRONT_RIGHT_LOWER";
            case ServoID::FRONT_LEFT_UPPER:  return "FRONT_LEFT_UPPER";
            case ServoID::FRONT_LEFT_LOWER:  return "FRONT_LEFT_LOWER";
            case ServoID::REAR_RIGHT_UPPER:  return "REAR_RIGHT_UPPER";
            case ServoID::REAR_RIGHT_LOWER:  return "REAR_RIGHT_LOWER";
            case ServoID::REAR_LEFT_UPPER:   return "REAR_LEFT_UPPER";
            case ServoID::REAR_LEFT_LOWER:   return "REAR_LEFT_LOWER";
            default:                         return "UNKNOWN_SERVO";
        }
    }

    const ServoID servos[8] = {
        ServoID::FRONT_RIGHT_LOWER,
        ServoID::FRONT_RIGHT_UPPER,
        ServoID::FRONT_LEFT_LOWER,
        ServoID::FRONT_LEFT_UPPER,
        ServoID::REAR_RIGHT_LOWER,
        ServoID::REAR_RIGHT_UPPER,
        ServoID::REAR_LEFT_LOWER,
        ServoID::REAR_LEFT_UPPER
    };

    void init();

    void moveServo(uint8_t id, short pos, int speed, int acc) {
        st.WritePosEx(id, pos, speed, acc);
    }
    void moveServo(ServoID servo, short pos, int speed, int acc) {
        moveServo(static_cast<uint8_t>(servo), pos, speed, acc);
    }

    void moveServos(const uint8_t ids[], size_t count, short positions[], int speed, int acc);
    void moveServos(const ServoID servos[], size_t count, short positions[], int speed, int acc);
    void stopServo(uint8_t id);
    void stopServo(ServoID servo) { stopServo(static_cast<uint8_t>(servo)); }
    void setTorque(uint8_t id, bool enabled);
    void setTorque(ServoID servo, bool enabled) { setTorque(static_cast<uint8_t>(servo), enabled); }
    void setAllTorque(bool enabled);

    int readLoad(uint8_t id) { return st.ReadLoad(id); }
    int readLoad(ServoID servo) { return readLoad(static_cast<uint8_t>(servo)); }

    int readPos(uint8_t id)  { return st.ReadPos(id); }
    int readPos(ServoID servo)  { return readPos(static_cast<uint8_t>(servo)); }

    int readVoltage(uint8_t id) { return st.ReadVoltage(id); }
    int readVoltage(ServoID servo) { return readVoltage(static_cast<uint8_t>(servo)); }

    int readCurrent(uint8_t id) { return st.ReadCurrent(id); }
    int readCurrent(ServoID servo) { return readCurrent(static_cast<uint8_t>(servo)); }

    int readTemper(uint8_t id) { return st.ReadTemper(id); }
    int readTemper(ServoID servo) { return readTemper(static_cast<uint8_t>(servo)); }

    int readTorque(uint8_t id) { return st.readByte(id, SMS_STS_TORQUE_ENABLE); }
    int readTorque(ServoID servo) { return readTorque(static_cast<uint8_t>(servo)); }

    bool isMoving(uint8_t id) { return st.ReadMove(id) != 0; }
    bool isMoving(ServoID servo) { return isMoving(static_cast<uint8_t>(servo)); }

private:
    SMS_STS st;
    Preferences pref;
};
