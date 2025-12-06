#include "gas_sensor.h"
#include "config.h"

// конструктор
// pin - номер вывода, куда подключен датчика газа
GasSensor::GasSensor(uint8_t pin) : mq135(pin) {}

// метод инициализации датчика газа
void GasSensor::initialize() {
    // калибровка датчика
    mq135.calibrate();
}

// метод, проверяющий интвервал для считывания нового значения
// принимает:
//      now - текущее время работы микроконтроллера
// возвращает:
//      true - время прошло
//      false - время еще не прошло
bool GasSensor::sampleReady(uint32_t now) {
    // прошло ли время для считывания нового значения
    if (now - lastSample >= SAMPLE_INTERVAL) {
        lastSample = now;
        return true;
    }
    return false;
}

// метод обработки данных с датчика
// возвращает:
//      true - превышен порог концентрации
//      false - порог не превышен
bool GasSensor::process() {
    // получение текущего времени работы микроконтроллера
    uint32_t now = millis();
    // если новое значение не готово
    // возвращаем старое
    if (!sampleReady(now)) return co2TooHigh;

    // читаем ppm co2
    co2 = mq135.readCO2();
    // если превышает порог
    if (co2 >= CO2_THRESHOLD) co2TooHigh = true;
    // иначе
    else co2TooHigh = false;
    
    return co2TooHigh;
}