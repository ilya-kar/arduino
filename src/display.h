#ifndef DISPLAY_H
#define DISPLAY_H

#include <GyverOLED.h>
#include "heart_sensor.h"

struct DisplayState {
    bool mpuAlarm = false;
    bool gasAlarm = false;
    bool defaultState = true;
    HeartSensor::HeartState maxState = HeartSensor::HeartState::NO_FINGER;
};

class Display {
public:
    Display();
    void updateAlarm(const DisplayState &state);
    void updateBpm(uint32_t bpm);
    void printData(const char *msg);

private:
    void clearForAlarm();
    void printDefault();
    void printGasAlarm();
    void printMpuAlarm();
    void printHeartHighAlarm();
    void printHeartLowAlarm();

    GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
    DisplayState lastState;
};

#endif //DISPLAY_H