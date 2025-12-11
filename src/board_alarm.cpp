#include "board_alarm.h"
#include "config.h"

// статическая переменная, отвечающая за нажатие кнопки
static volatile bool btnPressed = false;
// функция обработки прерывания
static void btnIsr() { btnPressed = true; }

void BoardAlarm::initialize() {
    // подтяжка резистором вывода под кнопку
    pinMode(PIN_BTN, INPUT_PULLUP);
    // настройка выводов под светодиоды
    pinMode(PIN_BUZZ, OUTPUT);
    pinMode(PIN_LED_GREEN_ALARM, OUTPUT);
    pinMode(PIN_LED_RED_ALARM, OUTPUT);

    // вкл зеленого состояния
    digitalWrite(PIN_LED_GREEN_ALARM, HIGH);

    // привязка обработчика прерывания к выводу кнопки
    attachInterrupt(digitalPinToInterrupt(PIN_BTN), btnIsr, FALLING);
}

// метод, позволяющий узнать, прошло ли время для переключения тревоги
// принимает:
//      now - текущее время работы микроконтроллера
// возвращает:
//      true - время прошло
//      false - время еще не прошло
bool BoardAlarm::isAlarmToggleTime(uint32_t now) {
    // прошло ли время для переключения
    if (now - tmrBuzzer >= (buzzerState ? BUZZER_ON : BUZZER_OFF)) {
        tmrBuzzer = now; // обновляем время
        buzzerState = !buzzerState; // меняем состояние зуммера
        return true;
    }
    return false;
}

// метод изменения состояния зуммера
// now - текущее время работы микроконтроллера
void BoardAlarm::changeMuteState(uint32_t now) {
    if (!flag) {
        flag = true;
        btnPressed = false;
        return;
    }

    // фильтрация дребезга кнопки
    if (now - lastBtnTime > DEBOUNCE_MS) {
        // вкл/выкл зуммера
        isMuted = !isMuted;
        lastBtnTime = now;
    }
    // сброс нажатия кнопки
    btnPressed = false;
}

// метод переключения тревоги
void BoardAlarm::toogleAlarm() {
    // получение текущего времени работы микроконтроллера
    uint32_t now = millis();
    // если кнопка была нажата
    if (btnPressed) changeMuteState(now);
    
    // если тревога не активна
    if (!alarmActive) {
        alarmActive = true;
        // выкл зеленого светодиода
        digitalWrite(PIN_LED_GREEN_ALARM, LOW);
    }

    // если прошло время для переключения тревоги
    if (isAlarmToggleTime(now)) {
        // если зуммер не выключен и текущее состояние воспроизведение тревоги
        // воспроизводим звук
        if (!isMuted && buzzerState) tone(PIN_BUZZ, BUZZER_FREQ);
        // иначе выключаем
        else noTone(PIN_BUZZ);
        // мигаем красным светодиодом
        digitalWrite(PIN_LED_RED_ALARM, buzzerState);
    }
}

// метод выключения тревоги
void BoardAlarm::disableAlarm() {
    // выкл воспроизведения тревоги зуммера
    noTone(PIN_BUZZ);
    // выкл красного светодиода
    digitalWrite(PIN_LED_RED_ALARM, LOW);
    // вкл зеленого светодиода
    digitalWrite(PIN_LED_GREEN_ALARM, HIGH);
    // сброс состояний
    buzzerState = false;
    alarmActive = false;
}