#include "gas_sensor.h"
#include "MPU6050_DMP.h"
#include "board_alarm.h"
#include "heart_sensor.h"
#include "display.h"

#define PIN_LED_INIT 5
#define PIN_MQ135 A0
#define PIN_DMP_ISR 3

GasSensor mq135(PIN_MQ135);
MPU6050_DMP mpu(PIN_DMP_ISR);
HeartSensor max30102;
BoardAlarm boardAlarm;
Display display;

void blink(int times, int on, int off);
void blinkInitLed();

void setup() {
    Serial.begin(115200);

    pinMode(PIN_LED_INIT, OUTPUT);
    blinkInitLed();

    if (!mpu.initialize()) {
        display.printData("Ошибка инициализации MPU-6050");
        while (1);
    }

    if (!max30102.initialize()) {
        display.printData("Ошибка инициализации MAX30102");
        while (1);
    }

    mq135.initialize();
}

void loop() {
    bool mpuState = mpu.process();
    bool mqState = mq135.process();
    HeartSensor::HeartState maxState = max30102.process();

    static uint32_t lastAvgBpm = -1;

    DisplayState state;
    state.mpuAlarm = mpuState;
    state.gasAlarm = mqState;
    state.defaultState = !(mqState || mpuState);

    if (lastAvgBpm != max30102.getAvgBpm()) display.updateBpm(max30102.getAvgBpm());
    display.updateAlarm(state);

    if (!state.defaultState) boardAlarm.toogleAlarm();
    else boardAlarm.disableAlarm();

    lastAvgBpm = max30102.getAvgBpm();
}

void blink(int times, int on, int off) {
    for (int i = 0; i < times; i++) {
        digitalWrite(PIN_LED_INIT, HIGH);
        delay(on);
        digitalWrite(PIN_LED_INIT, LOW);
        delay(off);
    }
}

void blinkInitLed() {
    blink(3, 500, 500);
    blink(2, 100, 100);
}