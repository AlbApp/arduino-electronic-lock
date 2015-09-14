/**
 * Электронный замок по карточкам
 * 
 * classes.h
 * 
 * @author: Ахрамеев Денис
 * @link: http://den.bz
 */

/**
 * Абстрактный класс состояния (паттерн проектирования State)
 */
class AState {
    public:
        virtual AState* open();
        virtual AState* close();
        virtual AState* move();
        virtual void fixPosition();
};
AState* AState::open() { return 0; }
AState* AState::close() { return 0; }

/**
 * Конкретное состояние: замок открыт
 */
class StateOpened: public AState {
    public:
        virtual AState* close();
        virtual AState* move();
        virtual void fixPosition();
};
AState* StateOpened::move() {
    return close();
}

/**
 * Конкретное состояние: замок закрыт
 */
class StateClosed: public AState {
    public:
        virtual AState* open();
        virtual AState* move();
        virtual void fixPosition();
};
AState* StateClosed::move() {
    return open();
}

/**
 * Замок
 */
class Lock {
    public:
        Lock();
        void open();
        void close();
        void move();
        void fixPosition();
    private:
        AState* _state;
};
Lock::Lock() {
    _state = new StateClosed;
}
void Lock::open() {
    _state = _state->open();
}
void Lock::close() {
    _state = _state->close();
}
void Lock::move() {
    _state = _state->move();
}
void Lock::fixPosition() {
    _state->fixPosition();
}

/**
 * Проигрователь (паттерн проектирования Singleton)
 */
class Player {
    public:
        static Player* getInstance();
        void playNotes(Note*, int);
    private:
        static Player* _instance;
};

Player* Player::_instance = 0;

/**
 * Возвращает указатель на экземпляр проигрователя
 * 
 * @return Player*
 */
Player* Player::getInstance() {
    if (_instance == 0) {
        _instance = new Player;
    }
    
    return _instance;
}

/**
 * Пропускной контроллёр
 */
class Controller {
    public:
        Controller(Lock*);
        void listenCards();
    protected:
        bool isGetCard();
        bool isAllowedCard(long);
    public:
        Lock* _lock;
};

