#pragma once

#include <inttypes.h>

typedef struct
{
    int x;
    int y;
    int width;
    int height;
} Rect;

class CDraft
{
public:
    CDraft(int width, int height);
    ~CDraft() {};
    void drawTile(uint16_t x, uint16_t y, uint8_t *tile, bool alpha = false) const;
    void fill(const uint16_t color) const;
    void drawFont(const int x, const int y, const char *s, uint8_t color) const;
    void drawRect(const Rect &rect, const uint8_t color, const bool fill = true) const;
    uint8_t *buf() { return m_buf; };
    int width() { return m_width; }
    int height() { return m_height; }

protected:
    uint8_t *m_buf = nullptr;
    int m_width;
    int m_height;
    enum
    {
        TILE_SIZE = 16,
        FONT_SIZE = 8,
        FONT_OFFSET = 8
    };
};
