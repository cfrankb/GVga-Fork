#pragma once

#include "pico/mutex.h"
#include "gvga.h"

class CGame;
class CAnimator;
class CDraft;
class IJoystick;

class CEngine
{
public:
    static CEngine *getEngine();
    ~CEngine();

    mutex_t *mutex();
    CGame &game();
    void drawScreen(GVga *gvga);
    void drawLevelIntro(GVga *gvga);
    void mainLoop(const uint32_t ticks);
    void attach(IJoystick *gamepad);

    enum
    {
        FONT_SIZE = 8,
        TILE_SIZE = 16,
        PLAYER_FRAMES = 8,
        PLAYER_REG_FRAMES = 7,
        PLAYER_SPEED = 3,
        NO_ANIMZ = 255,
        TILE_OFFSET = TILE_SIZE * TILE_SIZE,
        CONFIG_WIDTH = 320,
        CONFIG_HEIGHT = 200,
        BLACK = 0,
        WHITE = 1,
        YELLOW = 2,
        PURPLE = 3,
        GRAY = 4,
        LIGHTGRAY = 5,
        LIME = 6
    };

protected:
    CEngine();
    CAnimator *m_animator = nullptr;
    int m_playerFrameOffset = 0;
    int m_healthRef = 0;
    IJoystick *m_gamepad = nullptr;
    CGame *m_game = nullptr;
    bool init();
    void drawKeys(const CDraft &display, const int y);
    void drawBuffer(GVga *gvga, uint16_t orgX, uint16_t orgY, uint8_t *pixels, int len, int hei);
    void drawTile(GVga *gvga, int baseX, int baseY, uint8_t *tile, bool flip);
};
