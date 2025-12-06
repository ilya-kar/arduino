#include "display.h"

// битмап сердца
static const uint8_t icons_7x7[][7] PROGMEM {
    {0x0e, 0x1f, 0x3e, 0x7c, 0x3e, 0x1f, 0x0e},
    {0x0e, 0x11, 0x22, 0x44, 0x22, 0x11, 0x0e}
};

// битмап сердца
static const uint8_t icons_8x8[][8] PROGMEM {
    {0x1e, 0x3f, 0x7f, 0xfe, 0xfe, 0x7f, 0x3f, 0x1e}
};

// конструктор
Display::Display() {
    // инициализация дисплея
    oled.init();
    // очистка дисплея от мусора
    oled.clear();
    // печать текста по умолчанию
    updateBpm(0);
    printDefault();
}

// метод очистки дисплея под новую тревогу
void Display::clearForAlarm() {
    // установка шрифта в 3 раза больше обычного
    oled.setScale(3);
    // очистка области дисплея под тревогу
    oled.clear(0, 32, 128, 64);
    // установка курсора в нужную позицию
    oled.setCursor(0, 4);
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

// метод обновления значения пульса
// bpm - значение уд/мин
void Display::updateBpm(uint32_t bpm) {
    // если он ноль
    if (bpm == 0) {
        // пульс неизвестен
        oled.drawBitmap(48, 16, icons_7x7[1], 7, 7);
        oled.print(" N/A      ");
    } else {
        // иначе создаем эффект бьющего сердца в момент обновления пульса
        oled.drawBitmap(48, 16, icons_8x8[0], 8, 8);
        oled.print(" ");
        oled.print(bpm);
        oled.print(" уд/мин    ");
        oled.drawBitmap(48, 16, icons_7x7[0], 7, 7);
    }
}