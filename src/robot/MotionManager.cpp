#include "MotionManager.h"
#include <cmath>

void MotionManager::moveLeg(LEG leg, float x, float y, float z, int speed, int acc, bool useLegReference) {

    int legInt = static_cast<int>(leg);
    bool mirror = legInt % 2 == 0;

    uint8_t idLower = legInt * 2 + 1;
    uint8_t idUpper = legInt * 2 + 2;

    if (useLegReference) {
        toLegReference(leg, x, y);
    }

    short posUpper = 0, posLower = 0;

    if (!InverseKinematics::calcIK(posUpper, posLower, x, y, z, mirror)) {
        LOG("Attenzione: posizione fuori portata, spostamento al limite possibile");
    }

    uint8_t ids[2] = { idUpper, idLower };
    short positions[2] = { posUpper, posLower };
    sm.moveServos(ids, 2, positions, speed, acc);
}

void MotionManager::toLegReference(LEG leg, float& x, float& y) {
    switch (leg) {
        case LEG::FL: x = -x; break;
        case LEG::RR: y = -y; break;
        case LEG::RL: x = -x; y = -y; break;
        case LEG::FR: break;
    }
}

void MotionManager::moveAllLegs(float x, float y, float z, int speed, int acc, bool useLegReference) {
    moveLeg(LEG::FR, x, y, z, speed, acc, useLegReference);
    moveLeg(LEG::FL, x, y, z, speed, acc, useLegReference);
    moveLeg(LEG::RR, x, y, z, speed, acc, useLegReference);
    moveLeg(LEG::RL, x, y, z, speed, acc, useLegReference);
}



void MotionManager::debugFK(bool torque, FRAME frame) {
    for (int legInt = 0; legInt < 4; legInt++) {
        debugFK(static_cast<LEG>(legInt), torque, frame);
    }
}

void MotionManager::debugFK(LEG leg, bool torque, FRAME frame) {
    static const char* legNames[4] = { "FR", "FL", "RR", "RL" };

    int legInt = static_cast<int>(leg);
    bool mirror = legInt % 2 == 0;

    uint8_t idLower = legInt * 2 + 1;
    uint8_t idUpper = legInt * 2 + 2;

    sm.setTorque(idUpper, torque);
    sm.setTorque(idLower, torque);

    short posUpper = (short)sm.readPos(idUpper);
    short posLower = (short)sm.readPos(idLower);

    float x, y, z;
    InverseKinematics::calcFK(posUpper, posLower, x, y, z, mirror);

    const char* frameName = (frame == FRAME::BODY) ? "BODY" : "LEG";
    if (frame == FRAME::BODY) {
        switch (leg) {
            case LEG::FL: x = -x; break;
            case LEG::RR: y = -y; break;
            case LEG::RL: x = -x; y = -y; break;
            case LEG::FR: break;
        }
    }
    LOGF("LEG %s [%s] -> x: %.2f, y: %.2f, z: %.2f (posUpper: %d, posLower: %d)\n",
         legNames[legInt], frameName, x, y, z, posUpper, posLower);
}

void MotionManager::debugPower(LEG leg, bool torque) {
    static const char* legNames[4] = { "FR", "FL", "RR", "RL" };

    int legInt = static_cast<int>(leg);

    uint8_t idLower = legInt * 2 + 1;
    uint8_t idUpper = legInt * 2 + 2;

    if (torque) {
        sm.setTorque(idUpper, torque);
        sm.setTorque(idLower, torque);
    }

    int loadUpper = sm.readLoad(idUpper);
    int loadLower = sm.readLoad(idLower);
    int currUpper = sm.readCurrent(idUpper);
    int currLower = sm.readCurrent(idLower);
    int voltUpper = sm.readVoltage(idUpper);
    int voltLower = sm.readVoltage(idLower);

    LOGF("LEG %s [servo %d UPPER] load: %.1f%% (dir %+d), current: %d mA, voltage: %.2f V\n",
         legNames[legInt], idUpper, loadUpper / 10.0f, loadUpper, currUpper, voltUpper / 10.0f);
    LOGF("LEG %s [servo %d LOWER] load: %.1f%% (dir %+d), current: %d mA, voltage: %.2f V\n",
         legNames[legInt], idLower, loadLower / 10.0f, loadLower, currLower, voltLower / 10.0f);
}

void MotionManager::debugPower(bool torque) {
    for (int legInt = 0; legInt < 4; legInt++) {
        debugPower(static_cast<LEG>(legInt), torque);
    }
}


void MotionManager::readPos(LEG leg, short& posUpper, short& posLower) {
    int legInt = static_cast<int>(leg);

    uint8_t idLower = legInt * 2 + 1;
    uint8_t idUpper = legInt * 2 + 2;

    posUpper = (short)sm.readPos(idUpper);
    posLower = (short)sm.readPos(idLower);
}

void MotionManager::readPos(LEG leg, float& x, float& y, float& z) {
    int legInt = static_cast<int>(leg);
    bool mirror = legInt % 2 == 0;

    short posUpper, posLower;
    readPos(leg, posUpper, posLower);

    InverseKinematics::calcFK(posUpper, posLower, x, y, z, mirror);
}

bool MotionManager::isMoving(LEG leg) {
    int legInt = static_cast<int>(leg);

    uint8_t idLower = legInt * 2 + 1;
    uint8_t idUpper = legInt * 2 + 2;

    return sm.isMoving(idUpper) || sm.isMoving(idLower);
}

bool MotionManager::isMoving() {
    for (int legInt = 0; legInt < 4; legInt++) {
        if (isMoving(static_cast<LEG>(legInt))) return true;
    }
    return false;
}

void MotionManager::perform(MOVE move, int speed, int acc) {
    switch (move) {
            case MOVE::WAVE: performWave(speed, acc); break;
            case MOVE::OCTOPUS: performOctopus(speed, acc); break;
            case MOVE::STANDING: performStanding(speed, acc); break;
            case MOVE::LIE_DOWN: performLieDown(speed, acc); break;
            case MOVE::PURRING: performPurring(speed, acc); break;
            case MOVE::SWIMMING: performSwimming(speed, acc); break;
            case MOVE::PUSHUPS: performPushups(speed, acc); break;
            case MOVE::WALKING_F: currentMove = MOVE::WALKING_F; break;
            case MOVE::WALKING_B: currentMove = MOVE::WALKING_B; break;
            case MOVE::ROTATION_CW: currentMove = MOVE::ROTATION_CW; break;
            case MOVE::ROTATION_ACW: currentMove = MOVE::ROTATION_ACW; break;

            case MOVE::NONE: currentMove = MOVE::NONE; break;
            default: LOG("Move inesistente"); currentMove = MOVE::NONE; break;

    }
}
void MotionManager::performStanding(int speed, int acc) {
    speed = speed==0 ? 1100 : speed;
    acc = acc==0 ? 400 : acc;

    while (currentMove != MOVE::NONE) {}

    currentMove = MOVE::STANDING;

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);

    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    currentMove = MOVE::NONE;
}

void MotionManager::performLieDown(int speed, int acc) {
    speed = speed==0 ? 1100 : speed;
    acc = acc==0 ? 400 : acc;

    while (currentMove != MOVE::NONE) {}
    currentMove = MOVE::LIE_DOWN;

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);

    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    constexpr float x = -9.46, y = 9.21, z = 7.34;
    moveAllLegs(x, y, z, speed, acc, true);

    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    currentMove = MOVE::NONE;

}

void MotionManager::performWave(int speed, int acc) {

    speed = speed==0 ? 1300 : speed;
    acc = acc==0 ? 400 : acc;

    while (currentMove != MOVE::NONE) {}
    currentMove = MOVE::WAVE;

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);

    for (int i = 0; i <= 3; i++) {
        moveLeg(LEG::FR, 1.5, 4.9, 15.35, speed, acc);
        delay(10);
        while (isMoving(LEG::FR)) { delay(10); }
        moveLeg(LEG::FR, -4.5, 2.58, 15.35, speed, acc);
        delay(10);
        while (isMoving(LEG::FR)) { delay(10); }
    }

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    currentMove = MOVE::NONE;
}

void MotionManager::performOctopus(int speed, int acc) {

    speed = speed==0 ? 1300 : speed;
    acc = acc==0 ? 400 : acc;

    constexpr float x = -13.20f, y = -0.14f, z = 7.09f;
    constexpr float x2 = -5.34f, y2 = -0.09f, z2 = -0.65f;

    while (currentMove != MOVE::NONE) {}

    currentMove = MOVE::OCTOPUS;

    for (int i = 0; i <= 3; i++) {
        moveAllLegs(x, y, z, speed, acc, true);
        delay(10);
        while (isMoving()) { delay(10); }

        moveAllLegs(x2, y2, z2, speed, acc, true);
        delay(10);
        while (isMoving()) { delay(10); }
    }


    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    currentMove = MOVE::NONE;
}

void MotionManager::performPurring(int speed, int acc) {

    speed = speed==0 ? 1300 : speed;
    acc = acc==0 ? 200 : acc;

    while (currentMove != MOVE::NONE) {}
    currentMove = MOVE::PURRING;

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);

    constexpr float x = 2.86f, y = 9.68f, z = 13.68f;

    moveLeg(LEG::FR, x, y, z, speed, acc, true);
    moveLeg(LEG::FL, x, y, z, speed, acc, true);

    delay(10);
    while (isMoving()) { delay(10); }

    constexpr float x1 = 3.01, y1 = 10.31, z1 = 13.12, x2 = 2.17f, y2 = 7.42f, z2 = 14.94f;

    speed = 700;

    moveLeg(LEG::FL, x2, y2, z2, speed, acc, true);

    for (int i = 0; i <= 6; i++) {
        delay(10);
        while (isMoving()) { delay(10); }

        moveLeg(LEG::FL, x1, y1, z1, speed, acc, true);
        moveLeg(LEG::FR, x2, y2, z2, speed, acc, true);

        delay(10);
        while (isMoving()) { delay(10); }

        moveLeg(LEG::FL, x2, y2, z2, speed, acc, true);
        moveLeg(LEG::FR, x1, y1, z1, speed, acc, true);
    }

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    currentMove = MOVE::NONE;
}


void MotionManager::performSwimming(int speed, int acc) {
    speed = speed==0 ? 1300 : speed;
    acc = acc==0 ? 400 : acc;

    while (currentMove != MOVE::NONE) {}

    currentMove = MOVE::SWIMMING;

    constexpr float fx = -8.73f, fy = -9.9f, fz = 7.23f;
    constexpr float fx2 = -12.41f, fy2 = -4.5f, fz2 = 7.23f;
    constexpr float rx = -8.73f, ry = 9.9f, rz = 7.23f;
    constexpr float rx2 = -12.41f, ry2 = 4.5f, rz2 = 7.23f;

    for (int i = 0; i <= 4; i++) {
        moveLeg(LEG::FR, fx, fy, fz, speed, acc, true);
        moveLeg(LEG::FL, fx, fy, fz, speed, acc, true);
        moveLeg(LEG::RR, rx, ry, rz, speed, acc, true);
        moveLeg(LEG::RL, rx, ry, rz, speed, acc, true);
        delay(10);
        while (isMoving()) { delay(10); }

        moveLeg(LEG::FR, fx2, fy2, fz2, speed, acc, true);
        moveLeg(LEG::FL, fx2, fy2, fz2, speed, acc, true);
        moveLeg(LEG::RR, rx2, ry2, rz2, speed, acc, true);
        moveLeg(LEG::RL, rx2, ry2, rz2, speed, acc, true);
        delay(10);
        while (isMoving()) { delay(10); }
    }

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
    delay(10);
    while (isMoving(LEG::FR)) { delay(10); }

    currentMove = MOVE::NONE;
}

void MotionManager::performPushups(int speed, int acc) {
    speed = speed==0 ? 800 : speed;
    acc = acc==0 ? 100 : acc;

    constexpr float rx = 0.0f, ry = 5.0f, rz = 0.0f;
    constexpr float fx = -5.0f, fy = 0.0f, fz = 0.0f;
    constexpr float fx2 = -11.3f, fy2 = 0.0f, fz2 = 2.19f;

    while (currentMove != MOVE::NONE) {}
    currentMove = MOVE::PUSHUPS;

    moveLeg(LEG::FL, fx, fy, fz, speed, acc, true);
    moveLeg(LEG::FR, fx, fy, fz, speed, acc, true);
    moveLeg(LEG::RL, rx, ry, rz, speed, acc, true);
    moveLeg(LEG::RR, rx, ry, rz, speed, acc, true);

    delay(10);
    while (isMoving()) { delay(10); }

    for (int i = 0; i <= 7; i++) {

        moveLeg(LEG::FL, fx2, fy2, fz2, speed, acc, true);
        moveLeg(LEG::FR, fx2, fy2, fz2, speed, acc, true);

        delay(10);
        while (isMoving()) { delay(10); }

        moveLeg(LEG::FL, fx, fy, fz, speed, acc, true);
        moveLeg(LEG::FR, fx, fy, fz, speed, acc, true);

        delay(10);
        while (isMoving()) { delay(10); }
    }

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);

    delay(10);
    while (isMoving()) { delay(10); }

    currentMove = MOVE::NONE;

}

void MotionManager::performWalkingLoop() {

    constexpr int speed = 3400;
    constexpr int acc = 254;

    static bool initialized = false;
    static bool wasWalking = false;

    const bool isWalking = currentMove == MOVE::WALKING_F || currentMove == MOVE::WALKING_B;

    if (!initialized) {
        moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
        initialized = true;
    }

    if (!isWalking) {
        if (wasWalking) moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
            wasWalking = false;
            return;
        }

    wasWalking = true;

    constexpr float FR_X_1 = -10.96f; constexpr float FR_Y_1 = 7.35f; constexpr float FR_Z_1 = 7.50f;
    constexpr float FL_X_1 = -3.49f; constexpr float FL_Y_1 = 7.08f; constexpr float FL_Z_1 = -0.18f;
    constexpr float RR_X_1 = -4.51f; constexpr float RR_Y_1 = 0.68f; constexpr float RR_Z_1 = -0.62f;
    constexpr float RL_X_1 = -4.51f; constexpr float RL_Y_1 = 0.68f; constexpr float RL_Z_1 = -0.62f;

    constexpr float FR_X_2 = -8.42f; constexpr float FR_Y_2 = 5.67f; constexpr float FR_Z_2 = 1.06f;
    constexpr float FL_X_2 = -7.83f; constexpr float FL_Y_2 = 0.98f; constexpr float FL_Z_2 = -0.18f;
    constexpr float RR_X_2 = -2.89f; constexpr float RR_Y_2 = 3.53f; constexpr float RR_Z_2 = -0.62f;
    constexpr float RL_X_2 = -2.89f; constexpr float RL_Y_2 = 3.53f; constexpr float RL_Z_2 = -0.62f;

    bool forward = currentMove == MOVE::WALKING_F;

    const LEG LEG_A = forward ? LEG::FR : LEG::RR;
    const LEG LEG_B = forward ? LEG::FL : LEG::RL;
    const LEG LEG_C = forward ? LEG::RR : LEG::FR;
    const LEG LEG_D = forward ? LEG::RL : LEG::FL;

    // R
    moveLeg(LEG_A, FR_X_1, FR_Y_1, FR_Z_1, speed, acc, true);
    moveLeg(LEG_B, FL_X_1, FL_Y_1, FL_Z_1, speed, acc, true);
    moveLeg(LEG_C, RR_X_1, RR_Y_1, RR_Z_1, speed, acc, true);
    moveLeg(LEG_D, RL_X_1, RL_Y_1, RL_Z_1, speed, acc, true);

    delay(5);
    while (isMoving()) { }

    moveLeg(LEG_A, FR_X_2, FR_Y_2, FR_Z_2, speed, acc, true);
    moveLeg(LEG_B, FL_X_2, FL_Y_2, FL_Z_2, speed, acc, true);
    moveLeg(LEG_C, RR_X_2, RR_Y_2, RR_Z_2, speed, acc, true);
    moveLeg(LEG_D, RL_X_2, RL_Y_2, RL_Z_2, speed, acc, true);

    delay(5);
    while (isMoving()) { }

    // L
    moveLeg(LEG_B, FR_X_1, FR_Y_1, FR_Z_1, speed, acc, true);
    moveLeg(LEG_A, FL_X_1, FL_Y_1, FL_Z_1, speed, acc, true);
    moveLeg(LEG_D, RR_X_1, RR_Y_1, RR_Z_1, speed, acc, true);
    moveLeg(LEG_C, RL_X_1, RL_Y_1, RL_Z_1, speed, acc, true);

    delay(5);
    while (isMoving()) { }

    moveLeg(LEG_B, FR_X_2, FR_Y_2, FR_Z_2, speed, acc, true);
    moveLeg(LEG_A, FL_X_2, FL_Y_2, FL_Z_2, speed, acc, true);
    moveLeg(LEG_D, RR_X_2, RR_Y_2, RR_Z_2, speed, acc, true);
    moveLeg(LEG_C, RL_X_2, RL_Y_2, RL_Z_2, speed, acc, true);

    delay(5);
    while (isMoving()) { }


    // // S
    // moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
    //
    // delay(10);
    // while (isMoving()) { delay(10); }
}

void MotionManager::performRotationLoop() {

    constexpr int speed = 3400;
    constexpr int acc = 254;

    static bool initialized = false;
    static bool wasRotating = false;

    const bool isRotating = currentMove == MOVE::ROTATION_CW || currentMove == MOVE::ROTATION_ACW;

    if (!initialized) {
        moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
        initialized = true;
    }

    if (!isRotating) {
        if (wasRotating) moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);
        wasRotating = false;
        return;
    }

    wasRotating = true;
    const bool clockwise = currentMove == MOVE::ROTATION_CW;

    const float STANCE_X = 5.0f;
    const float STANCE_Y = clockwise ? 0.01f : -5.0f;
    const float STANCE_Z = 0.0f;
    const float SWING_X = -7.3f;
    const float SWING_Y = 6.52f;
    const float SWING_Z = 0.8f;

    moveAllLegs(STANDING_X, STANDING_Y, STANDING_Z, speed, acc, true);

    delay(5);
    while (isMoving()) { }

    moveLeg(LEG::FL, STANCE_X, STANCE_Y, STANCE_Z, speed, acc, true);
    moveLeg(LEG::RR, STANCE_X, STANCE_Y, STANCE_Z, speed, acc, true);

    moveLeg(LEG::FR, SWING_X, SWING_Y, SWING_Z, speed, acc, true);
    moveLeg(LEG::RL, SWING_X, SWING_Y, SWING_Z, speed, acc, true);

    delay(5);
    while (isMoving()) { }
}

void MotionManager::loop() {
    performWalkingLoop();
    performRotationLoop();
}
