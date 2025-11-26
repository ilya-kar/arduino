#include "MPU6050_DMP.h"

#define PITCH_LIMIT 35.0f
#define ROLL_LIMIT  30.0f

#define ENTER_DANGER_MS 250
#define EXIT_DANGER_MS 250

static volatile bool dataReady = false;
static void dmpIsr() { dataReady = true; }

MPU6050_DMP::MPU6050_DMP(uint8_t intPin) : intPin(intPin) {}

bool MPU6050_DMP::initialize() {
    Wire.begin();
    Wire.setClock(400000);

    mpu.initialize();
    if (!mpu.testConnection()) return false;

    if (mpu.dmpInitialize() != 0) return false;

    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);

    mpu.setDMPEnabled(true);

    attachInterrupt(digitalPinToInterrupt(intPin), dmpIsr, RISING);

    packetSize = mpu.dmpGetFIFOPacketSize();

    return true;
}

bool MPU6050_DMP::process() {
    if (!dataReady || !mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) return lastStableState;

    dataReady = false;

    Quaternion q;
    VectorFloat gravity;
    float ypr[3];

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    bool rawState = isDangerous(ypr);
    uint32_t now = millis();

    if (rawState != lastRawState) {
        lastRawState = rawState;
        stateChangeTime = now;
    }

    uint32_t requiredMs = rawState ? ENTER_DANGER_MS : EXIT_DANGER_MS;

    if (now - stateChangeTime >= requiredMs) lastStableState = rawState;

    return lastStableState;
}

bool MPU6050_DMP::isDangerous(float *ypr) {
    float pitch = degrees(ypr[1]);
    float roll = degrees(ypr[2]);

    return (abs(pitch) >= PITCH_LIMIT) || (abs(roll) >= ROLL_LIMIT);
}