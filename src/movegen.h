#pragma once

#include "defs.h"
#include "bitboard.h"
#include "movelist.h"


namespace MOVE_GEN {
    void generate_all_moves(MoveList &moveList, const Bitboard &b);
    void generate_captures_promotions(MoveList &moveList, const Bitboard &b);
}
