#include "board_alarm.h"

#define PIN_BTN 2
#define PIN_BUZZ 4
#define PIN_LED_GREEN_ALARM 6
#define PIN_LED_RED_ALARM 7

#define BUZZER_FREQ 1500
#define BUZZER_ON 100
#define BUZZER_OFF 100
#define BUZZER_MUTE_TIME 10000
#define DEBOUNCE_MS 50

static volatile bool btnPressed = false;
static void btnIsr() { btnPressed = true; }

BoardAlarm::BoardAlarm() {
    pinMode(PIN_BTN, INPUT_PULLUP);
    pinMode(PIN_BUZZ, OUTPUT);
    pinMode(PIN_LED_GREEN_ALARM, OUTPUT);
    pinMode(PIN_LED_RED_ALARM, OUTPUT);

    digitalWrite(PIN_LED_GREEN_ALARM, HIGH);

    attachInterrupt(digitalPinToInterrupt(PIN_BTN), btnIsr, FALLING);
}

bool BoardAlarm::isAlarmToggleTime(uint32_t now) {
    if (now - tmrBuzzer >= (buzzerState ? BUZZER_ON : BUZZER_OFF)) {
        tmrBuzzer = now;
        buzzerState = !buzzerState;
        return true;
    }
    return false;
}

void BoardAlarm::changeMuteState(uint32_t now) {
    if (now - lastBtnTime > DEBOUNCE_MS) {
        isMuted = !isMuted;
        lastBtnTime = now;
    }
    btnPressed = false;
}

void BoardAlarm::toogleAlarm() {
    uint32_t now = millis();
    if (btnPressed) changeMuteState(now);
    
    if (!alarmActive) {
        alarmActive = true;
        digitalWrite(PIN_LED_GREEN_ALARM, LOW);
    }

    if (isAlarmToggleTime(now)) {
        if (!isMuted && buzzerState) tone(PIN_BUZZ, BUZZER_FREQ);
        else noTone(PIN_BUZZ);
        digitalWrite(PIN_LED_RED_ALARM, buzzerState);
    }
}

void BoardAlarm::disableAlarm() {
    noTone(PIN_BUZZ);
    digitalWrite(PIN_LED_RED_ALARM, LOW);
    digitalWrite(PIN_LED_GREEN_ALARM, HIGH);
    buzzerState = false;
    alarmActive = false;
}