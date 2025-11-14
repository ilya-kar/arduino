#include <GyverOLED.h>
#include <MQ135.h>

#define PIN_BTN 2
#define PIN_LED_ALARM 3
#define PIN_BUZZ 5
#define PIN_MQ135 A0

#define CO2_THRESHOLD 1000
#define BUZZER_FREQ 1500
#define BUZZER_ON 100
#define BUZZER_OFF 100
#define BUZZER_MUTE_TIME 10000

volatile bool isMuted = false;

GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
MQ135 mq135(PIN_MQ135);

void btnIsr() { isMuted = true; }

void setup() {
    Serial.begin(115200);

    pinMode(PIN_BTN, INPUT_PULLUP);
    pinMode(PIN_LED_ALARM, OUTPUT);
    pinMode(PIN_BUZZ, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_BTN), btnIsr, FALLING);

    oled.init();
    oled.clear();
    oled.setScale(1);
    mq135.calibrate();
}

void loop() {
    static uint32_t tmrMQ;
    static uint32_t tmrBuzzer;
    static uint32_t tmrMute;
    static bool buzzerState;
    static bool isAirDirty = true;
    static float co2;

    uint32_t now = millis();

    if (isMuted && ((now - tmrMute)  >= BUZZER_MUTE_TIME)) {
        isMuted = false;
        tmrMute = now;
    }
    
    if ((now - tmrMQ) >= 250) {
        tmrMQ = now;
        co2 = mq135.readCO2();
    }

    if (co2 >= CO2_THRESHOLD) {
        if ((now - tmrBuzzer) >= (buzzerState ? BUZZER_ON : BUZZER_OFF)) {
            tmrBuzzer = now;
            buzzerState = !buzzerState;

            if (!isMuted && buzzerState) tone(PIN_BUZZ, BUZZER_FREQ);
            else noTone(PIN_BUZZ);

            digitalWrite(PIN_LED_ALARM, buzzerState);
        }

        if (!isAirDirty) {
            oled.clear();
            oled.setCursor(32, 3);
            oled.print("ГРЯЗНЫЙ ВОЗДУХ!");
            isAirDirty = true;
        }
    } else {
        noTone(PIN_BUZZ);
        digitalWrite(PIN_LED_ALARM, LOW);
        buzzerState = false;

        if (isAirDirty) {
            oled.clear();
            oled.setCursor(32, 3);
            oled.print("Все в норме");
            isAirDirty = false;
        }
    }
}