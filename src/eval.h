#pragma once
#include "board/bitboard.h"


namespace EVAL {
    const int PAWNPHASE = 0;
    const int KNIGHTPHASE = 1;
    const int BISHOPPHASE = 1;
    const int ROOKPHASE = 2;
    const int QUEENPHASE = 4;
    const int TOTALPHASE = (PAWNPHASE * 16 + KNIGHTPHASE * 4 + BISHOPPHASE * 4 + ROOKPHASE * 4 + QUEENPHASE * 2);

    inline int getPhase(const Board &b) {
        const BoardState &board = b.state;
        int phase = TOTALPHASE;
        phase -= (board.pieceCount[0] + board.pieceCount[1]) * PAWNPHASE;
        phase -= (board.pieceCount[2] + board.pieceCount[3]) * KNIGHTPHASE;
        phase -= (board.pieceCount[4] + board.pieceCount[5]) * BISHOPPHASE;
        phase -= (board.pieceCount[6] + board.pieceCount[7]) * ROOKPHASE;
        phase -= (board.pieceCount[8] + board.pieceCount[9]) * QUEENPHASE;

        return (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
    }

    void InitEval(std::string nnueFile);
    int evaluate(Board &board);
}

