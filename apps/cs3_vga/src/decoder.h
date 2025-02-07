#include <stdint.h>
#pragma once

class Decoder
{
public:
    Decoder()
    {
    }
    ~Decoder()
    {
    }

    inline void start(uint8_t *buf)
    {
        m_ptr = buf;
        m_idx = 0;
        m_count = 0;
        m_state = NOT_STARTED;
    }

    inline uint8_t get()
    {
        if (m_count == 0)
        {
            uint8_t c = m_ptr[m_idx];
            if (c >= 128) // compr
            {
                m_state = COMPR;
                m_count = c - 126;
            }
            else
            { // uncompr
                m_state = UNCOMPR;
                m_count = ++c;
            }
            ++m_idx;
        }

        uint8_t c = m_ptr[m_idx];
        --m_count;
        if (m_state == UNCOMPR || m_count == 0)
        {
            ++m_idx;
        }
        return c;
    }

private:
    enum
    {
        NOT_STARTED,
        COMPR,
        UNCOMPR,
    };
    uint8_t m_state = NOT_STARTED;
    uint8_t *m_ptr = nullptr;
    uint8_t m_count = 0;
    uint16_t m_idx = 0;
};