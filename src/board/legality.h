#pragma once
#include "bitboard.h"
#include "move.h"

namespace BITBOARD {

    bool InCheck(BoardState& bs);
    bool InCheckOther(BoardState& bs);
    bool isLegal(Board& b, MOVE move);
    bool can_castle_king(BoardState& bs);
    bool can_castle_queen(BoardState& bs);
    bool isPseudoLegal(BoardState& bs, MOVE move);

}
