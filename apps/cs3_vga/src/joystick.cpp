#include "joystick.h"
#include "hardware/gpio.h"

CGameController::CGameController(const uint8_t pinUp, const uint8_t pinDown, const uint8_t pinLeft, const uint8_t pinRight)
{
    m_buttonsGPIO[Up] = pinUp;
    m_buttonsGPIO[Down] = pinDown;
    m_buttonsGPIO[Left] = pinLeft;
    m_buttonsGPIO[Right] = pinRight;
}

CGameController::~CGameController()
{
}

uint16_t CGameController::read()
{
    uint8_t result = 0;
    uint8_t mask = 1;
    for (uint8_t i = 0; i < ButtonCount; ++i)
    {
        const auto &gpio = m_buttonsGPIO[i];
        if (gpio != NO_PIN)
        {
            result += gpio_get(gpio) * mask;
        }
        mask *= 2;
    }
    return result;
}

void CGameController::init()
{
    for (uint8_t i = 0; i < ButtonCount; ++i)
    {
        const auto &gpio = m_buttonsGPIO[i];
        if (gpio == NO_PIN)
            continue;
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_IN);
        // gpio_pull_up(gpio);
    }
}