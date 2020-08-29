#pragma once
#include "defs.h"
#include "piecesquaretable.h"
#include "magic_bitboards.h"



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
    int evaluate(int *material, uint64_t *pieces, Magics *magics, uint64_t *knightMoves, int *pieceCount, uint64_t occupied, bool col);
    int evaluate_debug(int *material, uint64_t *pieces, Magics *magics, uint64_t *knightMoves, int *pieceCount, uint64_t occupied);

private:

    void InitPieceBoards();
    void InitKingZoneMask();
    void InitPassedPawnsMask();
    void InitForwardBackwardMask();
    void InitDistanceArray();
    void InitOutpostSquares();
    void InitIsolatedPawnsMask();
    void InitOutpostMask();
    uint64_t adjacentMask(uint64_t pawns);
    int evaluate_piece_square_values(uint64_t *pieces, bool col);
    int evaluateTrappedRook(uint64_t *pieces, bool col);
    int evaluateImbalance(int *pieceCount, bool col);
    int evaluatePawns(uint64_t *pieces, bool col);
    int evaluatePassedPawns(uint64_t *pieces, bool col);
    int evaluateKnights(uint64_t *pieces, uint64_t *knightMoves, bool col);
    int evaluateBishops(uint64_t *pieces, Magics *magics, bool col);
    int evaluateRooks(uint64_t *pieces, Magics *magics, bool col);
    int evaluateQueens(uint64_t *pieces, Magics *magics, bool col);
    int evaluateKing(uint64_t *pieces, Magics *magics, bool col);
    int evaluatePawnShield(uint64_t *pieces, bool col);
    int pieceSquare[12][64];
    int pieceSquareEG[12][64];
    uint64_t kingZoneMask[2][64];
    uint64_t passedPawnMask[2][64];
    uint64_t forwardMask[2][64];
    int manhattanArray[64][64];
    int chebyshevArray[64][64];
    uint64_t isolatedPawnMask[64];
    uint64_t outpostMask[2][64];

    const int outpostPotential[2][64] = {{ 0,  0,  0,  0,  0,  0,  0,  0,
                                            0,  0,  0,  0,  0,  0,  0,  0,
                                            0,  0,  0,  0,  0,  0,  0,  0,
                                            0,  0,  5,  8,  8,  5,  0,  0,
                                            0,  5,  7, 10, 10,  7,  5,  0,
                                            0,  5,  8, 10, 10,  8,  5,  0,
                                            0,  2,  2,  5,  5,  2,  2,  0,
                                            0,  0,  0,  0,  0,  0,  0,  0},

                                          { 0,  0,  0,  0,  0,  0,  0,  0,
                                            0,  2,  2,  5,  5,  2,  2,  0,
                                            0,  5,  8, 10, 10,  8,  5,  0,
                                            0,  5,  7, 10, 10,  7,  5,  0,
                                            0,  0,  5,  8,  8,  5,  0,  0,
                                            0,  0,  0,  0,  0,  0,  0,  0,
                                            0,  0,  0,  0,  0,  0,  0,  0,
                                            0,  0,  0,  0,  0,  0,  0,  0}};



    // King safety
    const int pieceAttackWeight[8] = {0, 0, 50, 75, 88, 94, 97, 100};

    uint64_t attacksKnight[2];
    uint64_t attacksBishop[2];
    uint64_t attacksRook[2];
    uint64_t attacksQueen[2];

    uint64_t occupied;
    uint64_t unsafeSquares[2];
    int KSAttackersCount[2];
    int KSAttackersWeight[2];
    int KSAttacks[2];

    uint64_t mobilityUnsafeSquares[2];
    uint64_t minorUnsafe[2];
    uint64_t queenUnsafe[2];
    uint64_t tempUnsafe[2];


};
