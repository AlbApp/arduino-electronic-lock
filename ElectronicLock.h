/**
 * Электронный замок по карточкам
 * 
 * @author: Ахрамеев Денис
 * @link: http://den.bz
 */
#include "pitches.h"

#define RFID_RX_PIN 2 // RFID пины
#define RFID_TX_PIN 3 // RFID пины
#define BUZZER_PIN 5 // Пин пъезопищалки
#define SERVO_PIN 9 // Пин сервопривода
#define CLOSE_DEGREE 170 // Угол поворота сервопривода для закрытого состояния
#define OPEN_DEGREE 10 // Угол поворота сервопривода для открытого состояния
#define NOTE_PAUSE 0

// ИДЕНТИФИКАТОРЫ КАРТ
long allowedIds[] = {
    2570459,
    2571044
};

/**
 * Нота
 */
class Note {
    public:
        int frequency;
        int duration;
};

// Стартовая мелодия
Note startMelody[] = {
    {NOTE_E5, 300},
    {NOTE_DS5, 250},
    {NOTE_E5, 200},
    {NOTE_DS5, 200},
    {NOTE_E5, 200},
    {NOTE_B4, 200},
    {NOTE_D5, 200},
    {NOTE_C5, 200},
    {NOTE_A4, 500}
};

// Мелодия при открытии замка
Note openMelody[] = {
    {NOTE_C6, 150},
    {NOTE_PAUSE, 150},
    {NOTE_C6, 150}
};

// Мелодия при закрытии замка
Note closeMelody[] = {
    {NOTE_C6, 500}
};

// Мелодия при не разрешённой карте
Note wrongCardMelody[] = {
    {NOTE_D8, 2000}
};

// RFID библиотека в UART режиме
SeeedRFIDLib RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDTag tag;

// Сервопривод
Servo servo;

// Текущее положение
bool isClosed = true;

int degree;
