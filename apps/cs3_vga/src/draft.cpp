#include <algorithm>
#include <cstdio>
#include "draft.h"
#include "data.h"
#include "engine.h"

CDraft::CDraft(int width, int height)
{
    // ESP_LOGI(TAG, "Building Display Wrapper");
    m_buf = new uint8_t[width * height];
    m_width = width;
    m_height = height;
};

void CDraft::drawTile(uint16_t x, uint16_t y, const uint8_t *tile, bool alpha) const
{
    // ESP_LOGI(TAG, "Drawing tile at: %d %d %p", x, y, tile);
    uint8_t *d = m_buf + x + y * m_width;
    int i = 0;
    for (int yy = 0; yy < TILE_SIZE; ++yy)
    {
        for (int xx = 0; xx < TILE_SIZE; ++xx)
        {
            const auto pixel = tile[i++];
            if (pixel || !alpha)
            {
                d[xx + yy * m_width] = pixel;
            }
        }
    }
}

void CDraft::fill(const uint16_t color) const
{
    printf("Fill Buffer %d x %d\n", m_width, m_height);
    for (int i = 0; i < m_width * m_height; ++i)
    {
        m_buf[i] = color;
    }
}

void CDraft::drawFont(const int x, const int y, const char *s, uint8_t color) const
{
    const uint8_t *font = CEngine::dataPtr(BITFONT_BIN_OFFSET);
    for (int j = 0; s[j]; ++j)
    {
        int u = s[j] < 127 ? std::max(s[j] - ' ', 0) : 0;
        uint8_t *o = m_buf + x + y * m_width + FONT_SIZE * j;
        for (int yy = 0; yy < FONT_SIZE; ++yy)
        {
            uint8_t data = font[FONT_OFFSET * u + yy];
            for (int xx = 0; xx < FONT_SIZE; ++xx)
            {
                o[xx] = data & 1 ? color : 0;
                data = data >> 1;
            }
            o += m_width;
        }
    }
}

void CDraft::drawRect(const Rect &rect, const uint8_t color, const bool fill) const
{
    uint8_t *buf = m_buf + rect.x + rect.y * m_width;
    if (fill)
    {
        for (int y = 0; y < rect.height; ++y)
        {
            for (int x = 0; x < rect.width; ++x)
            {
                buf[x] = color;
            }
            buf += m_width;
        }
    }
    else
    {
        for (int y = 0; y < rect.height; ++y)
        {
            for (int x = 0; x < rect.width; ++x)
            {
                if (x == 0 || y == 0 || x == rect.width - 1 || y == rect.height - 1)
                {
                    buf[x] = color;
                }
            }
            buf += m_width;
        }
    }
}
