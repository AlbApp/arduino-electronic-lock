/**
 * Электронный замок по карточкам
 * 
 * @author: Ахрамеев Денис Викторович
 * @link: http://ahrameev.ru/project/arduino-lock.html
 */
 
#include <SoftwareSerial.h>
#include <SeeedRFIDLib.h>
#include <Servo.h>
#include "settings.h"
#include "classes.h"

/**
 * Открывает замок
 * 
 * @return State* новое состояние
 */
AState* StateClosed::open() {
    Player::getInstance()->playNotes(openMelody, sizeof(openMelody)/sizeof(Note));
    for (int degree = CLOSE_DEGREE; degree > OPEN_DEGREE; degree -= 10) {
        servo.write(degree);
        delay(15);
    }
    Serial.println("Opened.");
    return new StateOpened;
}

/**
 * Закрытие замка
 * 
 * @return State* новое состояние
 */
AState* StateOpened::close() {
    Player::getInstance()->playNotes(closeMelody, sizeof(closeMelody)/sizeof(Note));
    for (int degree = OPEN_DEGREE; degree < CLOSE_DEGREE; degree += 10) {
        servo.write(degree);
        delay(15);
    }
    Serial.println("Closed.");
    return new StateClosed;
}

/**
 * Поддержка позиции сервопривода
 */
void StateOpened::fixPosition() {
    servo.write(OPEN_DEGREE);
    delay(20);
}
void StateClosed::fixPosition() {
    servo.write(CLOSE_DEGREE);
    delay(20);
}

Controller::Controller(Lock* lock) {
    pinMode(BUZZER_PIN, OUTPUT);
    
    // Стартовая мелодия
    Player::getInstance()->playNotes(startMelody, sizeof(startMelody)/sizeof(Note));

    _lock = lock;
}

/**
 * Поднесена ли поддерживаемая RFID карта
 * 
 * @return bool
 */
bool Controller::isGetCard() {
    return RFID.isIdAvailable();
}

/**
 * Проигрывает мелодию из нот
 * 
 * @param Note* notes
 * @param int count количество нот
 */
void Player::playNotes(Note* notes, int count) {
    for (int i = 0; i < count; ++i) {
        if (notes[i].frequency != NOTE_PAUSE) {
            tone(BUZZER_PIN, notes[i].frequency, notes[i].duration);
        }
        delay(notes[i].duration);
    };
}

/**
 * Ищет карту в базе
 * 
 * @param long id идентификатор карты
 * @return bool
 */
bool Controller::isAllowedCard(long id) {
    int count = sizeof(allowedIds)/sizeof(long);
    for (int i = 0; i < count; ++i) {
        if (allowedIds[i] == id) {
            return true;
        }
    }

    return false;
}

/**
 * Ожидает получения идентификаторы карты, 
 * после чего выполняет перемещение задвижки
 */
void Controller::listenCards() {
    if (isGetCard()) {
        tag = RFID.readId();

        Serial.print("ID:       ");
        Serial.println(tag.id);
    
        // Ищем идентификатор в базе
        if (!isAllowedCard(tag.id)) {
            Serial.println("WARNING: Wrong card!");
            Player::getInstance()->playNotes(wrongCardMelody, sizeof(wrongCardMelody)/sizeof(Note));
            return;
        }

        _lock->move();
    }
}

Lock* LOCK;
Controller* CONTROLLER;

void setup() {
    // Инициализация
    Serial.begin(9600);
    Serial.println("Loading...");
    
    servo.attach(SERVO_PIN);
    servo.write(CLOSE_DEGREE);

    LOCK = new Lock;
    CONTROLLER = new Controller(LOCK);

    Serial.println("Ready.");
}

void loop() {
    CONTROLLER->listenCards();
    LOCK->fixPosition();
}
