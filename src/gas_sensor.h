#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include <MQ135.h>

class GasSensor {
public:
    GasSensor(uint8_t pin);
    void initialize();
    bool process();

private:
    bool sampleReady(uint32_t now);
    MQ135 mq135;
    uint32_t lastSample;
    uint32_t co2 = 0;
    bool co2TooHigh = false;
};

#endif //GAS_SENSOR_H