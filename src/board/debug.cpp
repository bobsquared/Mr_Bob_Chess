#include "debug.h"

namespace BITBOARD {

    // A debugging function for zobrist hashing.
    // A position key is created from scratch, and is compared to the iterative position key for confirmation.
    void debugZobristHash(BoardState& bs) {
        if (bs.posKey != ZOBRIST::hashBoard(bs.pieces, bs.castleRights, bs.enpassantSq, bs.toMove)) {
            std::cout << bs.posKey << " " << ZOBRIST::hashBoard(bs.pieces, bs.castleRights, bs.enpassantSq, bs.toMove) << std::endl;
        }
        assert (bs.posKey == ZOBRIST::hashBoard(bs.pieces, bs.castleRights, bs.enpassantSq, bs.toMove));
    }

}