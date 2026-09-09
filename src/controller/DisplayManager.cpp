#include "DisplayManager.h"

constexpr int SCREEN_WIDTH = 240;
constexpr int SCREEN_HEIGHT = 135;
constexpr uint32_t PERFORMANCE_CHANGE_INTERVAL = 200;
constexpr uint32_t STARTING_SCREEN_TIME = 3000;
constexpr int MIN_COORDS = 1000;
constexpr int MAX_COORDS = 3095;

constexpr int JOYSTICK_X_CENTER = 2551;
constexpr int JOYSTICK_Y_CENTER = 2518;
constexpr int JOYSTICK_DEAD_ZONE = 600;
constexpr uint8_t NO_COMMAND = 255;

const String PERFORMANCES[] = {"Standing", "Lie Down", "Wave", "Octopus", "Purring", "Swimming", "Pushups"};
constexpr unsigned int PERFORMANCES_COUNT = sizeof(PERFORMANCES) / sizeof(PERFORMANCES[0]);
int selectedPerformance = 0;

uint32_t now = 0;
uint32_t lastPerformanceChange = 0;

void DisplayManager::init() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    sprite.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    drawStartingSprite();
    now = millis();
}

void DisplayManager::loop() {
    int y = joystick.getY();
    uint32_t currentMillis = millis();
    bool pressed = joystick.isPressed();

    if (currentScreen == SCREEN::STARTING && currentMillis - now > STARTING_SCREEN_TIME) {
    	drawFaceSprite(FACE::STARRED);
    }

    if (currentScreen == SCREEN::FACE && joystick.isLongPressed() && !menuOpenedByLongPress) {
        drawPerfSelSprite();
        menuOpenedByLongPress = true;
    }

    if (currentScreen == SCREEN::PERFORMANCE_SELECTION && currentMillis - lastPerformanceChange >= PERFORMANCE_CHANGE_INTERVAL) {

        if (y < MIN_COORDS && selectedPerformance > 0) {
            selectedPerformance--;
            lastPerformanceChange = currentMillis;
            drawPerfSelSprite();
        } else if (y > MAX_COORDS && selectedPerformance < PERFORMANCES_COUNT - 1) {
            selectedPerformance++;
            lastPerformanceChange = currentMillis;
            drawPerfSelSprite();
        }
    }

    if (currentScreen == SCREEN::PERFORMANCE_SELECTION && pressed && !previousPressed && !menuOpenedByLongPress) {
        pendingCommand = static_cast<uint8_t>(selectedPerformance + 1);

    	switch (pendingCommand) {
			case 7: drawFaceSprite(FACE::STRONG_MUSCLES); break;
			case 2: drawFaceSprite(FACE::SLEEPY); break;
			case 5: drawFaceSprite(FACE::PURRING); break;
			default: drawFaceSprite(FACE::STARRED); break;
		}

    }

    if (!pressed) menuOpenedByLongPress = false;

    previousPressed = pressed;

}

uint8_t DisplayManager::getCommand() {

	static uint8_t joystickCommand = 0;

    if (pendingCommand != NO_COMMAND) {
        uint8_t command = pendingCommand;
        pendingCommand = NO_COMMAND;
        return command;
    }

    if (currentScreen != SCREEN::FACE) return NO_COMMAND;

	const int x = joystick.getX();
	const int y = joystick.getY();

	const int xOffset = x - JOYSTICK_X_CENTER;
	const int yOffset = y - JOYSTICK_Y_CENTER;

	const int absXOffset = abs(xOffset);
	const int absYOffset = abs(yOffset);

	if (absXOffset < JOYSTICK_DEAD_ZONE && absYOffset < JOYSTICK_DEAD_ZONE) {
		joystickCommand = 0;
		return 0;
	}

	if (joystickCommand != 0) return joystickCommand;

	if (absXOffset >= absYOffset) joystickCommand = xOffset > 0 ? 8 : 9;
	else joystickCommand = yOffset > 0 ? 10 : 11;

	return joystickCommand;

}

void DisplayManager::drawStartingSprite() {
    sprite.fillSprite(TFT_BLACK);

    constexpr int logoX = 15;
    constexpr int logoY = 38;
    constexpr int logoSize = 38;

    sprite.drawRoundRect(logoX, logoY, logoSize, logoSize, 9, TFT_RED);
    sprite.drawLine(logoX + 11, logoY + 27, logoX + 11, logoY + 11, TFT_RED);
    sprite.drawLine(logoX + 11, logoY + 11, logoX + 27, logoY + 27, TFT_RED);
    sprite.drawLine(logoX + 27, logoY + 27, logoX + 27, logoY + 11, TFT_RED);
    sprite.fillCircle(logoX + 27, logoY + 11, 2, TFT_RED);

    sprite.setTextDatum(TL_DATUM);
    sprite.setFreeFont(&FreeSansBold12pt7b);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("NikQX1", 62, 32);
    sprite.setTextColor(TFT_RED);
    sprite.drawString("CONTROLLER", 62, 62);

    sprite.setTextDatum(MC_DATUM);
    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("Nicolo' D'Agostino", SCREEN_WIDTH / 2, 106);
    sprite.drawFastHLine(24, 123, SCREEN_WIDTH - 48, TFT_RED);

    currentScreen = SCREEN::STARTING;
    sprite.pushSprite(0, 0);
}


void DisplayManager::drawPerfSelSprite() {

    currentScreen = SCREEN::PERFORMANCE_SELECTION;
    sprite.fillSprite(TFT_BLACK);

    sprite.setFreeFont(&FreeSansBold9pt7b);
    sprite.setTextDatum(TL_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("ANIMAZIONI", 14, 8);
    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.drawFastHLine(14, 31, SCREEN_WIDTH - 28, TFT_DARKGREEN);

    sprite.setFreeFont(&FreeSansBold12pt7b);
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(PERFORMANCES[selectedPerformance], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    uint16_t previousColor = selectedPerformance > 0 ? TFT_GREEN : TFT_DARKGREY;
    uint16_t nextColor = selectedPerformance < PERFORMANCES_COUNT - 1 ? TFT_GREEN : TFT_DARKGREY;

    sprite.drawRoundRect(8, 56, 11, 23, 4, previousColor);
    sprite.fillTriangle(11, 67, 16, 62, 16, 72, previousColor);
    sprite.drawRoundRect(221, 56, 11, 23, 4, nextColor);
    sprite.fillTriangle(229, 67, 224, 62, 224, 72, nextColor);

    sprite.setFreeFont(&FreeSansBold12pt7b);
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextDatum(MC_DATUM);

    constexpr int progressX = 70;
    constexpr int progressY = 116;
    constexpr int segmentWidth = 13;
    constexpr int segmentGap = 4;
    for (unsigned int i = 0; i < PERFORMANCES_COUNT; i++) {
        const uint16_t segmentColor = i == selectedPerformance ? TFT_GREEN : TFT_DARKGREY;
        sprite.fillRoundRect(progressX + i * (segmentWidth + segmentGap), progressY, segmentWidth, 4, 2, segmentColor);
    }

    sprite.pushSprite(0, 0);
}

void DisplayManager::drawFaceSprite(FACE face) {
	currentScreen = SCREEN::FACE;
	sprite.fillSprite(TFT_BLACK);

	const unsigned char* bitmap = nullptr;
	switch (face) {
		case FACE::STARRED: bitmap = BITMAP_STARRED_FACE; break;
		case FACE::PURRING: bitmap = BITMAP_PURRING_FACE; break;
		case FACE::TIRED: bitmap = BITMAP_TIRED_FACE; break;
		case FACE::SLEEPY: bitmap = BITMAP_SLEEPY_FACE; break;
		case FACE::STRONG_MUSCLES: bitmap = BITMAP_STRONG_MUSCLES; break;
	}

	sprite.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_WHITE);
	sprite.pushSprite(0, 0);

	if (face==FACE::STRONG_MUSCLES) {
		delay(2500);
		drawFaceSprite(FACE::TIRED);
	}

}
