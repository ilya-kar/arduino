#include "display.h"

static const uint8_t icons_7x7[][7] PROGMEM {
    {0x0e, 0x1f, 0x3e, 0x7c, 0x3e, 0x1f, 0x0e},
    {0x0e, 0x11, 0x22, 0x44, 0x22, 0x11, 0x0e}
};

static const uint8_t icons_8x8[][8] PROGMEM {
    {0x1e, 0x3f, 0x7f, 0xfe, 0xfe, 0x7f, 0x3f, 0x1e}
};

Display::Display() {
    oled.init();
    oled.clear();
    lastState = DisplayState();
}

void Display::clearForAlarm() {
    oled.setScale(3);
    oled.clear(0, 32, 128, 64);
    oled.setCursor(0, 4);
}

void Display::printData(const char *msg) {
    oled.clear();
    oled.home();
    oled.print(msg);
}

void Display::updateAlarm(const DisplayState &state) {
    if (state.mpuAlarm && !lastState.mpuAlarm) printMpuAlarm();
    else if (state.maxState == HeartSensor::HeartState::TOO_HIGH &&
             lastState.maxState != HeartSensor::HeartState::TOO_HIGH) printHeartHighAlarm();
    else if (state.maxState == HeartSensor::HeartState::TOO_LOW &&
             lastState.maxState != HeartSensor::HeartState::TOO_LOW) printHeartLowAlarm();
    else if (state.gasAlarm && !lastState.gasAlarm) printGasAlarm();
    else if (state.defaultState && !lastState.defaultState) printDefault();

    lastState = state;
}

void Display::printDefault() {
    clearForAlarm();
    oled.print("Все в порядке");
}

void Display::printGasAlarm() {
    clearForAlarm();
    oled.println("Высокое содержание");
    oled.print("CO2 в воздухе!");
}

void Display::printMpuAlarm() {
    clearForAlarm();
    oled.println("Низкая концетрация!");
}

void Display::printHeartHighAlarm() {
    clearForAlarm();
    oled.println("Высокий пульс!");
}

void Display::printHeartLowAlarm() {
    clearForAlarm();
    oled.println("Низкий пульс!");
}

void Display::updateBpm(uint32_t bpm) {
    if (bpm == 0) {
        oled.drawBitmap(48, 16, icons_7x7[1], 7, 7);
        oled.print(" N/A");
    } else {
        oled.drawBitmap(48, 16, icons_8x8[0], 8, 8);
        oled.print(" ");
        oled.print(bpm);
        oled.print(" уд/мин    ");
        oled.drawBitmap(48, 16, icons_7x7[0], 7, 7);
    }
}