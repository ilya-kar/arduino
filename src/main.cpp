// подключение библиотек
#include "gas_sensor.h"
#include "MPU6050_DMP.h"
#include "board_alarm.h"
#include "heart_sensor.h"
#include "display.h"

// инициализация глобальных переменных
GasSensor mq135(PIN_MQ135); // датчик газа
MPU6050_DMP mpu(PIN_DMP_ISR); // датчик с гироскопом и акселерометром
HeartSensor max30102; // датчик пульса
BoardAlarm boardAlarm; // сигнализация
Display display; // дисплей

// переменная для учета последнего значения уд/мин
uint32_t lastAvgBpm = 0;

// функция мигания светодиодом
void blink(int pin, int times, int on, int off);

// функция инициализации
void setup() {
    Serial.begin(115200);

    // светодиод инициализации
    pinMode(PIN_LED_INIT, OUTPUT);
    // мигаем 3 секунды с частотой 2 гц
    blink(PIN_LED_INIT, 3, 500, 500);

    display.initialize();

    // попытка инициализации MPU-6050
    if (!mpu.initialize()) {
        // если неудачно - зацикливаем
        display.printData("Ошибка инициализации\nMPU-6050");
        while (true);
    }

    // попытка инициализации MAX30102
    if (!max30102.initialize()) {
        // если неудачно - зацикливаем
        display.printData("Ошибка инициализации\nMAX30102");
        while (true);
    }

    // инициализация MQ-135
    mq135.initialize();

    boardAlarm.initialize();
}

// функция основного цикла
void loop() {
    // состояние по умолчанию - нет тревоги
    AlarmState state = AlarmState::DEFAULT_ALARM;

    // если воздух нечистый
    if (mq135.process()) state = AlarmState::GAS_ALARM;
    // если критический пульс
    if (max30102.process()) state = AlarmState::HEART_ALARM;
    // если угол наклона головы критический
    if (mpu.process()) state = AlarmState::MPU_ALARM;

   
    uint32_t avgBpm = max30102.getAvgBpm();
    float pitch = mpu.getPitch();
    float roll = mpu.getRoll();
    uint32_t co2 = mq135.getCo2();

    display.updateValues(avgBpm, pitch, roll, co2);

    // обновляем состояние тревоги на дисплее в порядке приоритета
    display.updateAlarm(state);

    // если есть тревога - включаем бортовую сигнализацию
    if (state != AlarmState::DEFAULT_ALARM) boardAlarm.toogleAlarm();
    // иначе выключаем
    else boardAlarm.disableAlarm();
}

// функция мигания светодиодом
// pin - номера вывода, куда подключен светодиод
// times - кол-во итераций мигания
// on, off - время вкл/выкл светодиода соответственно, мс
void blink(int pin, int times, int on, int off) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(on);
        digitalWrite(pin, LOW);
        delay(off);
    }
}