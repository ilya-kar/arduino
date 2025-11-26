#include "gas_sensor.h"

#define SAMPLE_INTERVAL 250
#define CO2_THRESHOLD 1000

GasSensor::GasSensor(uint8_t pin) : mq135(pin) {}

void GasSensor::initialize() {
    mq135.calibrate();
}

bool GasSensor::sampleReady(uint32_t now) {
    if (now - lastSample >= SAMPLE_INTERVAL) {
        lastSample = now;
        return true;
    }
    return false;
}

bool GasSensor::process() {
    uint32_t now = millis();
    if (!sampleReady(now)) return co2TooHigh;

    co2 = mq135.readCO2();
    if (co2 >= CO2_THRESHOLD) co2TooHigh = true;
    else co2TooHigh = false;
    
    return co2TooHigh;
}