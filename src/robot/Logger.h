#pragma once

#include <Arduino.h>

#define LOG_ENABLED

#ifdef LOG_ENABLED
    #define LOG(x) Serial.println(x)
    #define LOGF(...) Serial.printf(__VA_ARGS__)
#else
    #define LOG(x)
    #define LOGF(...)
#endif