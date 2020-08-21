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

    // Mobility
    const int knightMobilityBonus[9] =  {S(-38, -52), S(-25, -27), S(-8, -21), -S(-2, -8), S(7, 9), S(12, 13), S(18, 20), S(21, 21), S(24, 24)};
    const int bishopMobilityBonus[14] = {S(-25, -42), S(-11, -18), S(9, -6), S(17, 7), S(20, 14), S(22, 20), S(26, 24), S(27, 28), S(34, 36), S(35, 37), S(43, 47), S(48, 49), S(55, 56), S(58, 60)};
    const int rookMobilityBonus[15] =   {S(-29, -45), S(-11, -20), S(-5, 4), S(0, 10), S(2, 11), S(5, 15), S(8, 16), S(10, 18), S(11, 19), S(17, 25), S(24, 35), S(26, 42), S(28, 48), S(30, 52), S(42, 62)};
    const int queenMobilityBonus[27] =  {S(-15, -19), S(-15, -17), S(-10, -10), S(-7, -2), S(10, 15), S(11, 17), S(12, 19), S(13, 21), S(15, 25),
                                         S(18, 26), S(21, 29), S(23, 35), S(27, 39), S(27, 39), S(30, 45), S(32, 45), S(35, 49), S(35, 52), S(39, 55),
                                         S(39, 55), S(41, 59), S(47, 62), S(50, 65), S(55, 71), S(62, 76), S(66, 82), S(70, 88)};

    // King safety
    const int pieceAttackValue[5] = {0, 20, 25, 42, 75};
    const int pieceAttackWeight[8] = {0, 0, 50, 75, 88, 94, 97, 100};

    // Passed Pawn weights
    const int passedPawnWeight[7] = {S(0, 0), S(6, 12), S(8, 15), S(9, 16), S(32, 38), S(87, 96), S(122, 125)};

    // Doubled pawns and isolated pawns
    const int doublePawnValue = S(18, 12);
    const int isolatedPawnValue = S(10, 16);


    uint64_t occupied;
    uint64_t unsafeSquares[2];
    int KSAttackersCount[2];
    int KSAttackersWeight[2];

    uint64_t mobilityUnsafeSquares[2];
    uint64_t minorUnsafe[2];
    uint64_t queenUnsafe[2];
    uint64_t tempUnsafe[2];


};
