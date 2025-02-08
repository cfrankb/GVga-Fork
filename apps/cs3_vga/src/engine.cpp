
#include <cstring>
#include <string>
#include <stdint.h>
#include "engine.h"
#include "game.h"
#include "draft.h"
#include "tilesdata.h"
#include "animzdata.h"
#include "animator.h"
#include "data.h"
#include "gfx.h"

CDraft draft(CEngine::CONFIG_WIDTH, CEngine::TILE_SIZE);

auto_init_mutex(g_mutex);
CEngine *g_engine = nullptr;
CEngine *CEngine::getEngine()
{
    if (!g_engine)
    {
        g_engine = new CEngine();
    }
    return g_engine;
}

CEngine::CEngine()
{
    init();
}

CEngine::~CEngine()
{
    if (m_game)
    {
        delete m_game;
    }
}

CGame &CEngine::game()
{
    return *m_game;
}

void CEngine::drawLevelIntro(GVga *gvga)
{
    char t[20];
    switch (m_game->mode())
    {
    case CGame::MODE_INTRO:
        sprintf(t, "LEVEL %.2d", m_game->level() + 1);
        break;
    case CGame::MODE_RESTART:
        if (m_game->lives() == 1)
        {
            strcpy(t, "LAST LIFE !");
            break;
        }
        sprintf(t, "LIVES LEFT %.2d", m_game->lives());
        break;
    case CGame::MODE_GAMEOVER:
        strcpy(t, "GAME OVER");
    };

    uint16_t x = (CONFIG_WIDTH - strlen(t) * FONT_SIZE) / 2;
    uint16_t y = (CONFIG_HEIGHT - FONT_SIZE) / 2;
    gfx_clear(gvga, BLACK);
    draft.fill(BLACK);
    printf("Draw Level Intro at: %d, %d\n", x, y);
    draft.drawFont(x, 0, t, WHITE);
    drawBuffer(gvga, 0, y, draft.buf(), draft.width(), FONT_SIZE);
}

void CEngine::drawKeys(const CDraft &display, const int y)
{
    CGame &game = *m_game;
    int x = CONFIG_WIDTH - TILE_SIZE;
    const uint8_t *keys = game.keys();
    for (int i = 0; i < CGame::MAX_KEYS; ++i)
    {
        uint8_t k = keys[i];
        if (k)
        {
            display.drawTile(x, y, dataPtr(TILES_MCZ_OFFSET + k * TILE_OFFSET), true);
            x -= TILE_SIZE;
        }
    }
}

void CEngine::drawScreen(GVga *gvga)
{
    mutex_enter_blocking(&g_mutex);
    CMap &map = m_game->getMap();
    CActor &player = m_game->player();

    const int cols = CONFIG_WIDTH / TILE_SIZE;
    const int rows = CONFIG_HEIGHT / TILE_SIZE;
    const int lmx = std::max(0, player.getX() - cols / 2);
    const int lmy = std::max(0, player.getY() - rows / 2);
    const int mx = std::min(lmx, map.len() > cols ? map.len() - cols : 0);
    const int my = std::min(lmy, map.hei() > rows ? map.hei() - rows : 0);

    CActor *monsters;
    int count;
    m_game->getMonsters(monsters, count);

    for (uint16_t y = 0; y < rows; ++y)
    {
        if (y == rows - 1)
        {
            // draw bottom rect
            draft.drawRect(
                Rect{.x = 0, .y = 0, .width = CONFIG_WIDTH, .height = TILE_SIZE}, GRAY, true);
            draft.drawRect(
                Rect{.x = 0, .y = 0, .width = CONFIG_WIDTH, .height = TILE_SIZE}, LIGHTGRAY, false);

            // draw health bar
            draft.drawRect(
                Rect{.x = 4, .y = 4, .width = std::min(m_game->health() / 2, CONFIG_WIDTH - 4), .height = TILE_SIZE / 2}, LIME, true);
            draft.drawRect(
                Rect{.x = 4, .y = 4, .width = std::min(m_game->health() / 2, CONFIG_WIDTH - 4), .height = TILE_SIZE / 2}, WHITE, false);

            drawKeys(draft, 0 * TILE_SIZE);
            drawBuffer(gvga, 0, y * TILE_SIZE, draft.buf(), draft.width(), draft.height());
            continue;
        }

        for (uint16_t x = 0; x < cols; ++x)
        {
            uint16_t j;
            uint16_t i = y + my >= map.hei() ? TILES_BLANK : map.at(x + mx, y + my);
            if (i == TILES_ANNIE2)
            {
                const int frame = player.getAim() * PLAYER_FRAMES + m_playerFrameOffset;
                auto tile = dataPtr(ANNIE_MCZ_OFFSET + frame * TILE_OFFSET);
                if (y == 0)
                {
                    draft.drawTile(x * TILE_SIZE, 0, tile);
                }
                else
                {
                    drawTile(gvga, x * TILE_SIZE, y * TILE_SIZE, tile, false);
                }
            }
            else
            {
                if (i == TILES_STOP)
                {
                    i = TILES_BLANK;
                }
                j = m_animator->at(i);
                auto tile = j == NO_ANIMZ ? dataPtr(TILES_MCZ_OFFSET + i * TILE_OFFSET)
                                          : dataPtr(ANIMZ_MCZ_OFFSET + j * TILE_OFFSET);
                if (y == 0)
                {
                    draft.drawTile(x * TILE_SIZE, 0, tile);
                }
                else
                {
                    drawTile(gvga, x * TILE_SIZE, y * TILE_SIZE, tile, false);
                }
            }
        }

        const uint16_t offset = m_animator->offset() & 7;
        for (uint16_t i = 0; i < count; ++i)
        {
            const CActor &monster = monsters[i];
            if (monster.within(mx, my + y, mx + cols, my + y + 1))
            {
                const uint8_t tileID = map.at(monster.getX(), monster.getY());
                if (!m_animator->isSpecialCase(tileID))
                {
                    continue;
                }
                // special case animations
                const int16_t xx = monster.getX() - mx;
                auto tile = dataPtr(ANIMZ_MCZ_OFFSET + (monster.getAim() * 8 + ANIMZ_INSECT1 + offset) * TILE_OFFSET);
                if (y == 0)
                {
                    draft.drawTile(xx * TILE_SIZE, 0, tile);
                }
                else
                {
                    drawTile(gvga, xx * TILE_SIZE, y * TILE_SIZE, tile, false);
                }
            }
        }

        if (y == 0)
        {
            char tmp[16];
            uint16_t bx = 0;
            uint16_t offsetY = 0;
            uint16_t ssize = sprintf(tmp, "%.8d ", m_game->score());
            draft.drawFont(0, offsetY, tmp, WHITE);

            bx += ssize;
            ssize = sprintf(tmp, "DIAMONDS %.2d ", m_game->diamonds());
            draft.drawFont(bx * FONT_SIZE, offsetY, tmp, YELLOW);

            bx += ssize;
            sprintf(tmp, "LIVES %.2d ", m_game->lives());
            draft.drawFont(bx * FONT_SIZE, offsetY, tmp, PURPLE);
            drawBuffer(gvga, 0, 0, draft.buf(), draft.width(), draft.height());
        }
    }
    mutex_exit(&g_mutex);
}

bool CEngine::init()
{
    m_game = CGame::getGame();
    m_animator = new CAnimator();
    return true;
}

mutex_t *CEngine::mutex()
{
    return &g_mutex;
}

void CEngine::mainLoop(const uint32_t ticks)
{
    CGame &game = *m_game;
    if (game.mode() != CGame::MODE_LEVEL)
    {
        return;
    }

    const uint16_t joyState = 0;
    // m_gamepad ? m_gamepad->read() : 0; // readJoystick();
    if (ticks % PLAYER_SPEED == 0 && !game.isPlayerDead())
    {
        game.managePlayer(joyState);
    }
    else
    {
        game.killPlayer();
        if (game.isGameOver())
        {
            game.setMode(CGame::MODE_GAMEOVER);
        }
        else
        {
            game.loadLevel(true);
            // game.setMode(CGame::MODE_RESTART);
        }
    }

    if (ticks % PLAYER_SPEED == 0)
    {
        // ESP_LOGI(TAG, "joyState 0x%.4x", joyState);
        if (game.health() < m_healthRef && m_playerFrameOffset != PLAYER_REG_FRAMES)
        {
            m_playerFrameOffset = PLAYER_REG_FRAMES;
        }
        else if (joyState)
        {
            m_playerFrameOffset = (m_playerFrameOffset + 1) % PLAYER_REG_FRAMES;
        }
        else
        {
            m_playerFrameOffset = 0;
        }
        m_healthRef = game.health();
        m_animator->animate();
    }

    game.manageMonsters(ticks);

    if (!game.isGameOver())
    {
        if (game.goalCount() == 0)
        {
            m_healthRef = 0;
            mutex_enter_blocking(&g_mutex);
            game.nextLevel();
            mutex_exit(&g_mutex);
        }
    }
}

void CEngine::attach(IJoystick *gamepad)
{
    m_gamepad = gamepad;
}

void CEngine::drawBuffer(GVga *gvga, uint16_t baseX, uint16_t baseY, uint8_t *pixels, int len, int hei)
{
    for (int y = 0; y < hei; ++y)
    {
        auto p = gvga->drawFrame + gvga->rowBytes * (baseY + y);
        for (int x = 0; x < len; ++x)
        {
            p[baseX + x] = *(pixels++);
        }
    }
}

void CEngine::drawTile(GVga *gvga, uint16_t baseX, uint16_t baseY, const uint8_t *tile, bool flip)
{
    int i = 0;
    auto ptr = gvga->drawFrame + gvga->rowBytes * baseY + baseX;
    for (int y = 0; y < TILE_SIZE; ++y)
    {
        if (flip)
        {
            for (int x = TILE_SIZE - 1; x >= 0; --x)
            {
                ptr[x] = tile[i++];
            }
        }
        else
        {
            for (int x = 0; x < TILE_SIZE; ++x)
            {
                ptr[x] = tile[i++];
            }
        }
        ptr += gvga->rowBytes;
    }
}

const uint8_t *CEngine::dataPtr(const uint32_t offset)
{
    return reinterpret_cast<uint8_t *>(XIP_BASE + FLASH_TARGET_OFFSET + offset);
}
