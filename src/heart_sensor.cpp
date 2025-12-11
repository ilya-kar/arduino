#include "heart_sensor.h"
#include "config.h"

// метод инициализации датчика
// возвращает:
//      true - инициализация успешна
//      false - инициализация не успешна
bool HeartSensor::initialize() {
    // запуск шины i2c
    Wire.begin();
    // запуск датчика
    if (!max.begin(Wire, I2C_SPEED_FAST)) return false;

    // установка параметров датчика
    max.setup(0x1F, 4, 2, 400, 411);
    // обнуление массива значений
    for (int i = 0; i < RATE_SIZE; i++) rates[i] = 0;

    max.setPulseAmplitudeRed(0x0A);

    return true;
}

// метод обработки данных с датчика
// возвращает:
//      true - порог пульса превышен
//      false - порог пульса не превышен
bool HeartSensor::process() {
    // обновления данных
    max.check();

    // если данных нет
    // возвращаем пред состояние
    if (max.available() == 0) return lastState;

    // получение значения с ик светодиода
    int32_t irValue = max.getFIFOIR();
    // сдвиг к следующим данным
    max.nextSample();

    // если палец не приложен
    if (irValue < IR_NO_FINGER_THRESHOLD) {
        // делаем пульс 0
        avgBpm = 0;
        // старые значения не учитываем
        validRates = 0;
        return lastState = false;;
    }

    // проверяем, был ли удар
    if (checkForBeat(irValue)) {
        // высчитываем разницу от последнего зафиксированного удара
        uint32_t delta = millis() - lastBeatTime;
        lastBeatTime = millis();
        
        // вычисляем пульс
        float bpm = 60.f / (delta / 1000.f);
        
        // диапазон пульса
        if (bpm > 20 && bpm < 255) {
            // добавляем новое значение в массив
            rates[rateSpot++] = (uint8_t)bpm;
            // делаем индекс циклическим
            rateSpot %= RATE_SIZE;

            // пропускаем первые данные
            if (validRates < RATE_SIZE) validRates++;

            float sum = 0;
            // рассчет суммы значений массива
            for (uint8_t i = 0; i < validRates; i++) sum += rates[i];
            // рассчет среднего ариф пульса
            avgBpm = sum / validRates;
        }
    }

    // если пульс вышел за пределы нормального пульса
    // abnormal будет true
    bool abnormal = (avgBpm < BPM_MIN || avgBpm > BPM_MAX) && avgBpm != 0;
    return lastState = abnormal;
}