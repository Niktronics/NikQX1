#pragma once

class InverseKinematics {
public:
    static bool calcIK(short& posUpper, short& posLower, float x, float y, float z, bool mirror = false);
    static bool calcFK(short posUpper, short posLower, float& x, float& y, float& z, bool mirror = false);
    static void posToAngles(short posUpper, short posLower, float& degUpper, float& degLower, bool mirror = false);
};