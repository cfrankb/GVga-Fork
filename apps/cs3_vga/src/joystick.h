#pragma once

#include <stdint.h>

enum JoyDir
{
    JOY_UP = 0x01,
    JOY_DOWN = 0x02,
    JOY_LEFT = 0x04,
    JOY_RIGHT = 0x08,
    JOY_BUTTON = 0x10,
    JOY_A_BUTTON = 0x20,
    JOY_NONE = 0
};

#define NO_PIN -1

class IJoystick
{
protected:
    IJoystick() {};
    virtual ~IJoystick() {};
    enum
    {
        Up,
        Down,
        Left,
        Right,
        ButtonCount = 4
    };

public:
    virtual uint16_t read() = 0;
    virtual void init();
};

class CGameController : public IJoystick
{
public:
    CGameController(const uint8_t pinUp, const uint8_t pinDown, const uint8_t pinLeft, const uint8_t pinRight);
    ~CGameController() override;

    void init() override;
    uint16_t read() override;

private:
    uint8_t m_buttonsGPIO[ButtonCount];
};
