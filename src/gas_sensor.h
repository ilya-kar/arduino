#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

// подключение библиотек
#include <MQ135.h>

// класс датчика газа
class GasSensor {
public:
    GasSensor(uint8_t pin);
    // метод инициализации
    void initialize();
    // метод обработки данных с датчика
    bool process();

private:
    // метод, проверяющий интвервал для считывания нового значения
    bool sampleReady(uint32_t now);
    
    MQ135 mq135;
    uint32_t lastSample = 0;
    uint32_t co2 = 0;
    bool co2TooHigh = false;
};

#endif //GAS_SENSOR_H