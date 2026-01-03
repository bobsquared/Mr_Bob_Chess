#pragma once

#include "bitboard.h"
#include "magic_bitboards.h"

namespace BITBOARD {

    uint64_t isAttackedSee(BoardState& bs, int index);
    int seeCapture(BoardState& bs, MOVE capture);

}