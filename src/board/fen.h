#pragma once

#include "bitboard.h"

namespace BITBOARD {

    std::string posToFEN(BoardState& bs);
    void setPosFen(BoardState& bs, Accumulator<768, 768>& acc, std::string fen);

}