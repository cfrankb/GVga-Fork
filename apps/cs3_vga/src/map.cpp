#include "map.h"
#include <cstring>
#include <cstdio>
#include <algorithm>

static const char SIG[] = "MAPZ";
static const uint16_t VERSION = 0;
static const uint16_t MAX_SIZE = 256;

CMap::CMap(uint16_t len, uint16_t hei, uint8_t t)
{
    len = std::min(len, MAX_SIZE);
    hei = std::min(hei, MAX_SIZE);
    m_size = 0;
    m_len = len;
    m_hei = hei;
    m_map = (m_len * m_hei) != 0 ? new uint8_t[m_len * m_hei] : nullptr;
    if (m_map)
    {
        m_size = m_len * m_hei;
        memset(m_map, t, m_size * sizeof(m_map[0]));
    }
    else
    {
        printf("map is null. out of memory\n");
    }
};

CMap::~CMap()
{
    forget();
};

uint8_t &CMap::at(int x, int y)
{
    if (x >= m_len)
    {
        printf("x [%d] greater than m_len\n", x);
    }
    if (y >= m_hei)
    {
        printf("y [%d] greater than m_hei\n", y);
    }
    return *(m_map + x + y * m_len);
}

uint8_t *CMap::row(int y)
{
    return m_map + y * m_len;
}

void CMap::set(int x, int y, uint8_t t)
{
    at(x, y) = t;
}

void CMap::forget()
{
    if (m_map != nullptr)
    {
        delete[] m_map;
        m_map = nullptr;
        m_size = 0;
    }
    m_len = 0;
    m_hei = 0;
    m_size = 0;
    m_attrs.clear();
}

uint16_t CMap::len() const
{
    return m_len;
}

uint16_t CMap::hei() const
{
    return m_hei;
}

bool CMap::resize(uint16_t len, uint16_t hei, bool fast)
{
    len = std::min(len, MAX_SIZE);
    hei = std::min(hei, MAX_SIZE);
    // printf("len:%d hei:%d fast:%d\n", len, hei, fast);
    if (fast)
    {
        if (len * hei > m_size)
        {
            printf("resizing map\n");
            forget();
            m_map = new uint8_t[len * hei];
            if (m_map == nullptr)
            {
                m_lastError = "resize fail";
                printf("%s\n", m_lastError.c_str());
                return false;
            }
        }
        m_attrs.clear();
    }
    else
    {
        uint8_t *newMap = new uint8_t[len * hei];
        memset(newMap, 0, len * hei);
        for (int y = 0; y < std::min(m_hei, hei); ++y)
        {
            memcpy(newMap + y * len,
                   m_map + y * m_len,
                   std::min(len, m_len));
        }
        delete[] m_map;
        m_map = newMap;
    }
    m_size = len * hei;
    m_len = len;
    m_hei = hei;
    return true;
}

const Pos CMap::findFirst(uint8_t tileId)
{
    for (uint16_t y = 0; y < m_hei; ++y)
    {
        for (uint16_t x = 0; x < m_len; ++x)
        {
            if (at(x, y) == tileId)
            {
                return Pos{x, y};
            }
        }
    }
    return Pos{NOT_FOUND, NOT_FOUND};
}

int CMap::count(uint8_t tileId)
{
    int count = 0;
    for (uint16_t y = 0; y < m_hei; ++y)
    {
        for (uint16_t x = 0; x < m_len; ++x)
        {
            if (at(x, y) == tileId)
            {
                ++count;
            }
        }
    }
    return count;
}

void CMap::clear(uint8_t ch)
{
    if (m_map && m_len * m_hei > 0)
    {
        for (int i = 0; i < m_len * m_hei; ++i)
        {
            m_map[i] = ch;
        }
    }
    m_attrs.clear();
}

uint8_t CMap::getAttr(const uint8_t x, const uint8_t y)
{
    const uint16_t key = toKey(x, y);
    if (m_attrs.size() > 0 && m_attrs.count(key) != 0)
    {
        return m_attrs[key];
    }
    else
    {
        return 0;
    }
}
void CMap::setAttr(const uint8_t x, const uint8_t y, const uint8_t a)
{
    const uint16_t key = toKey(x, y);
    if (a == 0)
    {
        m_attrs.erase(key);
    }
    else
    {
        m_attrs[key] = a;
    }
}

const char *CMap::lastError()
{
    return m_lastError.c_str();
}

int CMap::size()
{
    return m_size;
}

CMap &CMap::operator=(const CMap &map)
{
    forget();
    m_len = map.m_len;
    m_hei = map.m_hei;
    m_size = map.m_len * map.m_hei;
    m_map = new uint8_t[m_size];
    memcpy(m_map, map.m_map, m_size);
    m_attrs = map.m_attrs;
    return *this;
}

uint16_t CMap::toKey(const uint8_t x, const uint8_t y)
{
    return x + (y << 8);
}
