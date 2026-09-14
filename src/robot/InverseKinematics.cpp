#include "InverseKinematics.h"
#include <cmath>

const float L1 = 5.2f;
const float L2 = 8.0f;
const float Hb = 7.35f;

constexpr float RAD_TO_STEPS = 4095.0f / (2.0f * M_PI);

bool InverseKinematics::calcIK(short& posUpper, short& posLower, float x, float y, float z, bool mirror) {
    float zRel = z - Hb;
    float R = sqrtf(x * x + y * y);
    
    float targetY = mirror ? -y : y;
    float targetX = mirror ? -x : x;

    float dx = R - L1;
    float distSq = dx * dx + zRel * zRel;
    float dist = sqrtf(distSq);

    bool reachable = true;

    const float EPSILON = 0.05f;

    if (dist > L2 + EPSILON) {
        reachable = false;
        float scale = L2 / dist;
        dx = dx * scale;
        zRel = zRel * scale;
    } else if (dist > L2) {
        float scale = L2 / dist;
        dx = dx * scale;
        zRel = zRel * scale;
    }

    float thetaLower = atan2f(targetY, targetX);
    float thetaUpper = atan2f(zRel, dx);

    int pU = (int) lroundf(2048 + thetaUpper * RAD_TO_STEPS);
    int pL = (int) lroundf(2048 + thetaLower * RAD_TO_STEPS);

    if (pU < 0) { pU = 0; reachable = false; }
    if (pU > 4095) { pU = 4095; reachable = false; }
    if (pL < 0) { pL = 0; reachable = false; }
    if (pL > 4095) { pL = 4095; reachable = false; }

    posUpper = (short) pU;
    posLower = (short) pL;

    return reachable;
}

bool InverseKinematics::calcFK(short posUpper, short posLower, float& x, float& y, float& z, bool mirror) {
    float thetaUpper = (float) (posUpper - 2048) / RAD_TO_STEPS;
    float thetaLower = (float) (posLower - 2048) / RAD_TO_STEPS;

    float R = L1 + L2 * cosf(thetaUpper);
    float zRel = L2 * sinf(thetaUpper);

    float localX = R * cosf(thetaLower);
    float localY = R * sinf(thetaLower);

    if (mirror) {
        x = -localX;
        y = -localY;
    } else {
        x = localX;
        y = localY;
    }
    
    z = zRel + Hb;

    return true;
}

void InverseKinematics::posToAngles(short posUpper, short posLower, float& degUpper, float& degLower, bool mirror) {
    float thetaUpper = (float) (posUpper - 2048) / RAD_TO_STEPS;
    float thetaLower = (float) (posLower - 2048) / RAD_TO_STEPS;

    degUpper = thetaUpper * 180.0f / (float)M_PI;
    degLower = thetaLower * 180.0f / (float)M_PI;
}