#pragma once

#include <inttypes.h>

class CAnimator
{
public:
    CAnimator();
    ~CAnimator();
    void animate();
    uint8_t at(uint8_t tileID);
    uint8_t offset();
    bool isSpecialCase(uint8_t tileID);

protected:
    typedef struct
    {
        uint8_t srcTile;
        uint8_t startSeq;
        uint8_t count;
    } animzSeq_t;

    enum : uint32_t
    {
        NO_ANIMZ = 255,
        MAX_TILES = 256
    };

    const static animzSeq_t m_animzSeq[];
    uint8_t m_tileReplacement[MAX_TILES];
    uint8_t *m_seqIndex = nullptr;

    uint8_t m_offset = 0;
};
