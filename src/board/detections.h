#pragma once

#include "bitboard.h"

namespace BITBOARD {

    bool noPotentialWin(BoardState& bs);
    bool isDraw(BoardState& bs, MoveInfoStack& moveHistory, int ply);

}