#pragma once

#include <C:\Users\user\Documents\PlatformIO\Projects\NikQX1\.pio\libdeps\controller\TFT_eSPI\TFT_eSPI.h>
#include <Joystick.h>
#include "images/STARRED_FACE_BITMAP.h"
#include "images/PURRING_FACE_BITMAP.h"
#include "images/TIRED_FACE_BITMAP.h"
#include "images/SLEEPY_FACE_BITMAP.h"
#include "images/STRONG_MUSCLES_BITMAP.h"

class DisplayManager {
    public:
        DisplayManager(Joystick& js) : joystick(js) {};
        void init();
        void loop();
		uint8_t getCommand();
    private:
        Joystick& joystick;
        TFT_eSPI tft;
        TFT_eSprite sprite = TFT_eSprite(&tft);
        
        enum class SCREEN {
            NONE, STARTING, PERFORMANCE_SELECTION, FACE
        };

		enum class FACE {
			STARRED, PURRING, TIRED, SLEEPY, STRONG_MUSCLES
		};

		SCREEN currentScreen = SCREEN::NONE;
		uint8_t pendingCommand = 255;
		bool menuOpenedByLongPress = false;
		bool previousPressed = false;

		void drawStartingSprite();
        void drawPerfSelSprite();
		void drawFaceSprite(FACE face);
};
