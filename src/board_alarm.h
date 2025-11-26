#ifndef BOARD_ALARM_H
#define BOARD_ALARM_H

#include <Arduino.h>

class BoardAlarm {
public:
    BoardAlarm();
    void toogleAlarm();
    void disableAlarm();

private:
    bool isAlarmToggleTime(uint32_t now);
    void changeMuteState(uint32_t now);

    uint32_t tmrBuzzer = 0;
    uint32_t lastBtnTime = 0;
    bool buzzerState = false;
    bool alarmActive = false;
    bool isMuted = false;
};

#endif //BOARD_ALARM_H