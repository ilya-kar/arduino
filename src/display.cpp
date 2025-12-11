#include "display.h"

// битмап сердца
static const uint8_t icons_7x7[][7] PROGMEM {
    {0x0e, 0x1f, 0x3e, 0x7c, 0x3e, 0x1f, 0x0e},
    {0x0e, 0x11, 0x22, 0x44, 0x22, 0x11, 0x0e}
};

void Display::initialize() {
    // инициализация дисплея
    oled.init();
    // очистка дисплея от мусора
    oled.clear();
    // печать текста по умолчанию
    updateValues(0, 0, 0, 0);
    printDefault();
}

// метод очистки дисплея под новую тревогу
void Display::clearForAlarm() {
    // очистка области дисплея под тревогу
    oled.clear(0, 32, 128, 64);
    // установка курсора в нужную позицию
    oled.setCursor(0, 5);
}

// метод печати текста на дисплей
// msg - сообщение
void Display::printData(const char *msg) {
    oled.clear();
    oled.home();
    oled.print(msg);
}

// метод обновления текста на дисплее при новой тревоге
// state - какой из датчиков дал тревогу
void Display::updateAlarm(const AlarmState &state) {
    // если предыдущее состояние не изменилось
    // не дергаем дисплей
    if (state == lastState) return;

    // в зависимости от сработанного датчика и приоритета
    // печатаем соответствующий текст
    if (state == AlarmState::MPU_ALARM) printMpuAlarm();
    else if (state == AlarmState::HEART_ALARM) printHeartAlarm();
    else if (state == AlarmState::GAS_ALARM) printGasAlarm();
    else if (state == AlarmState::DEFAULT_ALARM) printDefault();

    // сохраняем новое состояние
    lastState = state;
}

// метод печати сообщения тревоги
// line1, line2 - строки текста
void Display::showAlarmMessage(const char* line1, const char* line2) {
    // очистка области дисплея
    clearForAlarm();
    // печать текста
    if (line1) oled.println(line1);
    if (line2) oled.println(line2);
}

// метод печати текста, когда тревоги нет
void Display::printDefault() {
    showAlarmMessage("Все в порядке");
}

// метод печати текста, когда тревога связана с датчиком газа
void Display::printGasAlarm() {
    showAlarmMessage("Высокое содержание", "CO2 в воздухе!");
}

// метод печати текста, когда тревога связана с MPU-6050
void Display::printMpuAlarm() {
    showAlarmMessage("Низкая концентрация!");
}

// метод печати текста, когда тревога связана с датчиком пульса
void Display::printHeartAlarm() {
    showAlarmMessage("Опасный пульс!");
}

void Display::updateValues(uint32_t bpm, float pitch, float roll, uint32_t co2) {
    if (bpm > 0 && lastBpm == 0) {
        oled.drawBitmap(0, 0, icons_7x7[0], 7, 7);
    } else if (bpm == 0 && lastBpm != 0) {
        oled.drawBitmap(0, 0, icons_7x7[1], 7, 7);
    }

    oled.setCursor(8, 0);
    if (bpm != lastBpm) {
        if (bpm == 0) {
            oled.print(" N/A        ");
        } else {
            oled.print(" ");
            oled.print(bpm);
            oled.print(" уд/мин     ");
        }
    }

    if (millis() - tmrAngle > 100) {
        oled.setCursor(0, 1);
        if (pitch != lastPitch) {
            oled.print("pitch: ");
            oled.print(pitch);
            oled.print(" градусов       ");
        }

        oled.setCursor(0, 2);
        if (roll != lastRoll) {
            oled.print("roll: ");
            oled.print(roll);
            oled.print(" градусов       "); 
        }

        tmrAngle = millis();
    }

    oled.setCursor(0, 3);
    if (co2 != lastCo2) {
        oled.print("co2 ppm: ");
        oled.print(co2);
        oled.print("    ");
    }

    lastBpm = bpm;
    lastPitch = pitch;
    lastRoll = roll;
    lastCo2 = co2;
}