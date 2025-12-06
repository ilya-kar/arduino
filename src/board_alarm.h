#ifndef BOARD_ALARM_H
#define BOARD_ALARM_H

// подключение библиотек
#include <Arduino.h>

// класс бортовой тревоги
class BoardAlarm {
public:
    BoardAlarm();
    // метод переключения тревоги
    void toogleAlarm();
    // метод отключения тревоги
    void disableAlarm();

private:
    // метод, позволяющий узнать, прошло ли время для переключения тревоги
    bool isAlarmToggleTime(uint32_t now);
    // метод изменения состояния зуммера
    void changeMuteState(uint32_t now);

    uint32_t tmrBuzzer = 0;
    uint32_t lastBtnTime = 0;
    bool buzzerState = false;
    bool alarmActive = false;
    bool isMuted = false;
};

#endif //BOARD_ALARM_H