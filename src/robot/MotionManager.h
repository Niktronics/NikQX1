#pragma once
#include <cstdint>
#include <InverseKinematics.h>
#include <ServoManager.h>
#include <Arduino.h>

class MotionManager {


    public:
        MotionManager(ServoManager& servoManager) : sm(servoManager) {}

        enum class LEG : uint8_t { FR, FL, RR, RL };
        enum class MOVE : uint8_t { NONE, STANDING, LIE_DOWN, WAVE, OCTOPUS, PURRING, SWIMMING, PUSHUPS, WALKING_F, WALKING_B, ROTATION_CW, ROTATION_ACW };
        MOVE currentMove = MOVE::NONE;

        void moveLeg(LEG leg, float x, float y, float z, int speed, int acc, bool useLegReference = false);
        void moveAllLegs(float x, float y, float z, int speed, int acc, bool useLegReference = false);
        void perform(MOVE move, int speed = 0, int acc = 0);

        enum class FRAME : uint8_t { LEG, BODY };
        void debugFK(bool torque = false, FRAME frame = FRAME::BODY);
        void debugFK(LEG leg, bool torque = false, FRAME frame = FRAME::BODY);

        void debugPower(bool torque = false);
        void debugPower(LEG leg, bool torque = false);

        void readPos(LEG leg, float& x, float& y, float& z);
        void readPos(LEG leg, short& posUpper, short& posLower);

        bool isMoving(LEG leg);
        bool isMoving();

        void loop();

    private:
        ServoManager& sm;

        const float STANDING_X = -5.03, STANDING_Y = 3.87, STANDING_Z = -0.57;

        void toLegReference(LEG leg, float& x, float& y);
        void performStanding(int speed, int acc);
        void performLieDown(int speed, int acc);
        void performWave(int speed, int acc);
        void performOctopus(int speed, int acc);
        void performPurring(int speed, int acc);
        void performSwimming(int speed, int acc);
        void performPushups(int speed, int acc);
        void performWalkingLoop();
        void performRotationLoop();

};
