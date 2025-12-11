#include "MPU6050_DMP.h"
#include "config.h"

// флаг готовности новых данных от DMP
static volatile bool dataReady = false;
// обработчик прерывания от MPU-6050
static void dmpIsr() { dataReady = true; }

// конструктор
// pin - номер вывода, куда подключено прерывание от MPU-6050
MPU6050_DMP::MPU6050_DMP(uint8_t intPin) : intPin(intPin) {}

// метод инициализации датчика
// возвращает:
//      true - инициализация успешна
//      false - инициализация не успешна
bool MPU6050_DMP::initialize() {
    // запуск шины i2c
    Wire.begin();
    // установка высокой скорости для i2c
    Wire.setClock(400000);

    // инициализация MPU-6050
    mpu.initialize();


    // инициализация DMP (встроенный цифровой процессор ориентации)
    if (mpu.dmpInitialize() != 0) return false;

    // калибровка акселерометра и гироскопа
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);

    // вкл DMP
    mpu.setDMPEnabled(true);

    // привязка обработчика прерывания готовности данных от DMP
    attachInterrupt(digitalPinToInterrupt(intPin), dmpIsr, RISING);

    return true;
}

// метод обработки данных с датчика
// возвращает:
//      true - порог угла наклона превышен
//      false - порог угла наклона не превышен
bool MPU6050_DMP::process() {
    // если данных нет
    // возвращаем пред состояние
    if (!dataReady || !mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) return lastStableState;

    // сброс флага данных
    dataReady = false;

    // буферы под данные ориентации
    Quaternion q;
    VectorFloat gravity;
    // yaw, pitch, roll
    float ypr[3];

    // получаем ориентацию в радианах
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    // определяем, опасное ли положение головы
    bool rawState = isDangerous(ypr);
    uint32_t now = millis();

    // фиксируем момент, когда состояние изменилось (для антидребезга)
    if (rawState != lastRawState) {
        lastRawState = rawState;
        stateChangeTime = now;
    }

    // разное время на вход/выход в/из опасного состояния
    uint32_t requiredMs = rawState ? ENTER_DANGER_MS : EXIT_DANGER_MS;

    // если состояние удерживается достаточно долго - считаем его стабильным
    if (now - stateChangeTime >= requiredMs) lastStableState = rawState;

    return lastStableState;
}

// метод, позволяющий узнать, является ли угол наклона критическим
// принимает:
//      ypt - указатель на массив значений yaw, pitch, roll
// возвращает:
//      true - порог угла наклона превышен
//      false - порог угла наклона не превышен
bool MPU6050_DMP::isDangerous(const float *ypr) {
    // вычисление абсолютного угла наклона вперед/назад в градусах
    pitch = abs(degrees(ypr[1]));
    // вычисление абсолютного угла наклона влево/вправо в градусах
    roll  = abs(degrees(ypr[2]));
    // вычисление превышения порога
    return (pitch >= PITCH_LIMIT) || (roll >= ROLL_LIMIT);
}
