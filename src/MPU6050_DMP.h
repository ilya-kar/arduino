#ifndef MPU6050_DMP_H
#define MPU6050_DMP_H

// подключение библиотек
#include "MPU6050_6Axis_MotionApps612.h"

// класс MPU-6050
class MPU6050_DMP {
public:
    MPU6050_DMP(uint8_t intPin);
    // метод инициализации датчика
    bool initialize();
    // метод обработки данных с датчика
    bool process();

private:
    // метод, позволяющий узнать, является ли угол наклона критическим
    bool isDangerous(const float *ypr);

    MPU6050 mpu;
    uint8_t intPin;
    uint16_t packetSize;
    uint8_t fifoBuffer[64];
    bool lastStableState = false;
    bool lastRawState = false;
    uint32_t stateChangeTime = 0;
};

#endif //MPU6050_DMP_H