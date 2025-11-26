#ifndef HEART_SENSOR_H
#define HEART_SENSOR_H

#include "MAX30105.h"
#include "heartRate.h"

class HeartSensor {
public:
    enum HeartState {
        NO_FINGER,
        OK,
        TOO_LOW,
        TOO_HIGH
    };

    bool initialize();
    HeartState process();
    uint32_t getAvgBpm() const { return avgBPM; }

private:
    static const uint8_t RATE_SIZE = 4;
    MAX30105 max;
    uint32_t lastBeatTime = 0;
    uint8_t rates[RATE_SIZE];
    uint8_t rateSpot = 0;
    uint32_t avgBPM = 0;
    HeartState lastState;
};

#endif //HEART_SENSOR_H