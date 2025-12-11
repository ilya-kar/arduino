#ifndef DISPLAY_H
#define DISPLAY_H

// подключение библиотек
#include <GyverOLED.h>
#include "heart_sensor.h"
#include "config.h"

// класс дисплея
class Display {
public:
    void initialize();
    // метод обновления текста тревоги на дисплее
    void updateAlarm(const AlarmState &state);
    // метод обновления пульса
    void updateValues(uint32_t bpm, float pitch, float roll, uint32_t co2);
    // метод печати текста
    void printData(const char *msg);

private:
    // методы печати сообщений на дисплее
    void showAlarmMessage(const char* line1, const char* line2 = nullptr);
    void clearForAlarm();
    void printDefault();
    void printGasAlarm();
    void printMpuAlarm();
    void printHeartAlarm();

    GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
    AlarmState lastState = AlarmState::DEFAULT_ALARM;
    uint32_t lastBpm = -1;
    float lastPitch = -1;
    float lastRoll = -1;
    uint32_t lastCo2 = -1;
    uint32_t tmrAngle = 0;
};

#endif //DISPLAY_H