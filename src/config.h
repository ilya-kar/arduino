#ifndef CONFIG_G
#define CONFIG_G

// main
// вывод светодиода инициализации
#define PIN_LED_INIT 5
// вывод датчика газа
#define PIN_MQ135 A0
// вывод прерывания для MPU-6050 (готовность данных)
#define PIN_DMP_ISR 3

// board_alarm
// вывод кнопки вкл/выкл зумера
#define PIN_BTN 2
// вывод зуммера
#define PIN_BUZZ 4
// вывод зеленого светодиода (все в норме)
#define PIN_LED_GREEN_ALARM 6
// вывод красного светодиода (тревога)
#define PIN_LED_RED_ALARM 7
// частота звука для зуммера, гц
#define BUZZER_FREQ 1500
// время работы зуммера в вкл состоянии, мс
#define BUZZER_ON 100
// время работы зуммера в выкл состоянии, мс
#define BUZZER_OFF 100
// время фильтрации дребезга кнопки, мс
#define DEBOUNCE_MS 50

// gas_sensor
// интервал считывания нового значения с датчика газа
#define SAMPLE_INTERVAL 250
// порог концентрации co2 в воздухе
#define CO2_THRESHOLD 1000

// heart_sensor
// порог значения датчика пульса, если отсутствует палец
#define IR_NO_FINGER_THRESHOLD 50000
// порог мин пульса
#define BPM_MIN 50
// порог макс пульса
#define BPM_MAX 120

// mpu
// порог угла наклона головы вперед/назад (тангаж), градусы
#define PITCH_LIMIT 35.0f
// порог угла наклона головы влево/вправо (крен), градусы
#define ROLL_LIMIT  30.0f
// фильтрация от быстрых входов/выходов в/из состояние опасно
#define ENTER_DANGER_MS 250
#define EXIT_DANGER_MS 250

// enum alarm
// перечесление для приоритета сигнализации
enum class AlarmState {
    DEFAULT_ALARM,
    MPU_ALARM,
    HEART_ALARM,
    GAS_ALARM
};

#endif //CONFIG_G