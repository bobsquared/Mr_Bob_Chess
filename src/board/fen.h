#pragma once

#include "bitboard.h"

namespace BITBOARD {

    std::string posToFEN(BoardState& bs);
    void setPosFen(BoardState& bs, Accumulator& acc, std::string fen);

}