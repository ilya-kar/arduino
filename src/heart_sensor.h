#ifndef HEART_SENSOR_H
#define HEART_SENSOR_H

// подключение библиотек
#include "MAX30105.h"
#include "heartRate.h"

// класс датчика пульса
class HeartSensor {
public:
    // метод инициализации
    bool initialize();
    // метод обработки данных с датчика
    bool process();
    // геттер среднего значения пульса
    uint32_t getAvgBpm() const { return avgBpm; }

private:
    // размер для усредняющего массива
    static const uint8_t RATE_SIZE = 4;
    MAX30105 max;
    uint32_t lastBeatTime = 0;
    // массив значений
    uint8_t rates[RATE_SIZE];
    // текущая позиция в массиве
    uint8_t rateSpot = 0;
    uint8_t validRates = 0;
    // среднее значение пульса
    uint32_t avgBpm = 0;
    bool lastState = false;
};

#endif //HEART_SENSOR_H