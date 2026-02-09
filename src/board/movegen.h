#pragma once

#include "../defs.h"
#include "legality.h"
#include "bitboard.h"
#include "movelist.h"


namespace MOVE_GEN {
    void generate_all_moves(MoveList &moveList, BoardState &b);
    void generate_captures_promotions(MoveList &moveList, BoardState &b);
    void generate_quiets(MoveList &moveList, BoardState &b);
}
