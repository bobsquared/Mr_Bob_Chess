#pragma once
#include "defs.h"
#include "magic_bitboards.h"
#include "bitboard.h"
#include "nnue/KPNNUE.h"



#define PAWNPHASE   0
#define KNIGHTPHASE 1
#define BISHOPPHASE 1
#define ROOKPHASE   2
#define QUEENPHASE  4
#define TOTALPHASE (PAWNPHASE * 16 + KNIGHTPHASE * 4 + BISHOPPHASE * 4 + ROOKPHASE * 4 + QUEENPHASE * 2)


#define S(mg, eg) ((int)((unsigned int)(mg) << 16) + (eg))
#define MGVAL(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))
#define EGVAL(s) ((int16_t)((uint16_t)((unsigned)((s)))))


class Eval {

public:

    Eval();
    int evaluate(Bitboard &board, KPNNUE *model);
    int scaleEndgame(Bitboard &board, int eval);

private:
    void InitializeEval(Bitboard &board, ThreadSearch *th);
    void InitLightSquares();
    void InitDistanceArray();
    int getPhase(Bitboard &board);

    uint64_t lightSquares;
    int manhattanArray[64][64];
    int chebyshevArray[64][64];


};
