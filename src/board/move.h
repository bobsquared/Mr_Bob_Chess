#pragma once

#include "bitboard.h"
#include "magic_bitboards.h"
#include "../transpositionTable.h"

namespace BITBOARD {

    namespace detail {
        void make_move_impl(BoardState& bs, MoveInfoStack& moveHistory, Accumulator<768, 768>& acc, MOVE move);
        void undo_move_impl(BoardState& bs, MoveInfoStack& moveHistory, Accumulator<768, 768>& acc, MOVE move);
        void make_null_move_impl(BoardState& bs, MoveInfoStack& moveHistory);
        void undo_null_move_impl(BoardState& bs, MoveInfoStack& moveHistory);
    }

    inline void make_move(Board& b, MOVE move) {
        detail::make_move_impl(b.state, b.moveHistory, b.acc, move);
        TT::prefetchTT(b.state.posKey);
    }

    inline void undo_move(Board& b, MOVE move) {
        detail::undo_move_impl(b.state, b.moveHistory, b.acc, move);
    }

    inline void make_null_move(Board& b) {
        detail::make_null_move_impl(b.state, b.moveHistory);
        TT::prefetchTT(b.state.posKey);
    }

    inline void undo_null_move(Board& b) {
        detail::undo_null_move_impl(b.state, b.moveHistory);
    }

}