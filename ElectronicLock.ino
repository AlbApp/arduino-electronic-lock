/**
 * Электронный замок по карточкам
 * 
 * @author: Ахрамеев Денис
 * @link: http://den.bz
 */
#include <SoftwareSerial.h>
#include <SeeedRFIDLib.h>
#include <Servo.h>
#include "ElectronicLock.h"

/**
 * Проигрывает мелодию из нот
 * 
 * @param Note* notes
 * @param int count количество нот
 */
void playNotes(Note* notes, int count) 
{
    for (int i = 0; i < count; ++i) {
        if (notes[i].frequency != NOTE_PAUSE) {
            tone(BUZZER_PIN, notes[i].frequency, notes[i].duration);
        }
        delay(notes[i].duration);
    };
}

/**
 * Перемещает положение задвижки
 * 
 * @param bool toClose закрыть ли
 */
void moving(bool toClose)
{
    if (!toClose) {
        playNotes(openMelody, sizeof(openMelody)/sizeof(Note));
        for (degree = CLOSE_DEGREE; degree > OPEN_DEGREE; degree -= 10) {
            servo.write(degree);
            delay(15);
        }
        Serial.println("Opened.");
    } else {
        playNotes(closeMelody, sizeof(closeMelody)/sizeof(Note));
        for (degree = OPEN_DEGREE; degree < CLOSE_DEGREE; degree += 10) {
            servo.write(degree);
            delay(15);
        }
        Serial.println("Closed.");
    }
}

/**
 * Поддерживает позицию сервопривода
 */
void fixPosition()
{
    if (isClosed) {
        servo.write(CLOSE_DEGREE);
    } else {
        servo.write(OPEN_DEGREE);
    }
    delay(20);
}

/**
 * Ищет идентификатор в базе
 * 
 * @param long id
 * @return bool
 */
bool isAllowedId(long id)
{
    for (int i = 0; i < sizeof(allowedIds)/sizeof(long); ++i) {
        if (allowedIds[i] == tag.id) {
            return true;
        }
    }

    return false;
}

/*
 * Основная логика работы замка
 */
bool lockWork()
{
    if (RFID.isIdAvailable()) {
        tag = RFID.readId();
        Serial.print("ID:       ");
        Serial.println(tag.id);

        // Ищем идентификатор в базе
        if (!isAllowedId(tag.id)) {
            playNotes(wrongCardMelody, sizeof(wrongCardMelody)/sizeof(Note));
            Serial.println("WARNING: Wrong card!");
            return false;
        }

        isClosed = !isClosed;

        moving(isClosed);
    }

    return true;
}

void loop() 
{  
    lockWork();

    // Поддержка положения
    fixPosition();
}

void setup() 
{
    // Инициализация
    Serial.begin(9600);
    Serial.println("Loading...");
    
    servo.attach(SERVO_PIN);
    servo.write(CLOSE_DEGREE);
    
    pinMode(BUZZER_PIN, OUTPUT);

    // Стартовая мелодия
    playNotes(startMelody, sizeof(startMelody)/sizeof(Note));

    Serial.println("Ready.");
}

