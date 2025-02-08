#pragma once

#include "pico/mutex.h"
#include "gvga.h"
#include "tilesdata.h"

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
    static const uint8_t *dataPtr(const uint32_t offset);

    enum
    {
        FLASH_TARGET_OFFSET = 0x00042000,
        FLASH_BUFFER = 256,
        TILES_MAX = TILES_YELKILLER,
        FONT_SIZE = 8,
        TILE_SIZE = 16,
        PLAYER_FRAMES = 8,
        PLAYER_REG_FRAMES = 7,
        PLAYER_SPEED = 3,
        NO_ANIMZ = 255,
        TILE_OFFSET = TILE_SIZE * TILE_SIZE,
        CONFIG_WIDTH = 320,
        CONFIG_HEIGHT = 240,
        BLACK = 0x00,
        WHITE = 0x1f,
        YELLOW = 0x36,
        PURPLE = 0x3a,
        GRAY = 0x46,
        LIGHTGRAY = 0x60,
        LIME = 0x93
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
    void drawTile(GVga *gvga, int baseX, int baseY, const uint8_t *tile, bool flip);
};
