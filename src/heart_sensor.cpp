#include "heart_sensor.h"

#define IR_NO_FINGER_THRESHOLD 50000
#define BPM_MIN 50
#define BPM_MAX 120

bool HeartSensor::initialize() {
    Wire.begin();
    if (!max.begin(Wire, I2C_SPEED_FAST)) return false;

    max.setup(0x1F, 4, 2, 400, 411);
    for (int i = 0; i < RATE_SIZE; i++) rates[i] = 0;

    return true;
}

HeartSensor::HeartState HeartSensor::process() {
    max.check();

    if (max.available() == 0) return lastState;

    int32_t irValue = max.getFIFOIR();
    max.nextSample();

    if (irValue < IR_NO_FINGER_THRESHOLD) {
        avgBPM = 0;
        return (lastState = NO_FINGER);
    }

    if (checkForBeat(irValue)) {
        uint32_t delta = millis() - lastBeatTime;
        lastBeatTime = millis();
        
        float bpm = 60.f / (delta / 1000.f);
        
        if (bpm > 20 && bpm < 255) {
            rates[rateSpot++] = (uint8_t)bpm;
            rateSpot %= RATE_SIZE;

            float sum = 0;
            for (uint8_t i = 0; i < RATE_SIZE; i++) sum += rates[i];
            avgBPM = sum / RATE_SIZE;
        }
    }

    if (avgBPM < BPM_MIN && avgBPM != 0) return (lastState = TOO_LOW);
    if (avgBPM > BPM_MAX) return (lastState = TOO_HIGH);

    return (lastState = OK);
}