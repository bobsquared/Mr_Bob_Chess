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


const int pieceValues[6] = {S(85, 100), S(305, 305), S(315, 315), S(475, 535), S(925, 985), S(2000, 2000)};

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
    int evaluateMobility(uint64_t *pieces, Magics *magics, uint64_t *knightMoves, uint64_t occupied, bool col);
    int evaluateKingSafety(uint64_t *pieces, Magics *magics, uint64_t *knightMoves, uint64_t occupied, bool col);
    int evaluateImbalance(int *pieceCount, bool col);
    int evaluatePawns(uint64_t *pieces, bool col);
    int evaluatePassedPawns(uint64_t *pieces, bool col);
    int evaluateOutposts(uint64_t *pieces, bool col);
    int pieceSquare[12][64];
    int pieceSquareEG[12][64];
    uint64_t kingZoneMask[2][64];
    uint64_t passedPawnMask[2][64];
    uint64_t forwardMask[2][64];
    int manhattanArray[64][64];
    int chebyshevArray[64][64];
    uint64_t isolatedPawnMask[64];
    uint64_t outpostMask[2][64];

    const uint64_t outpostPotential[2][64] = {{ 0,  0,  0,  0,  0,  0,  0,  0,
                                                0,  0,  0,  0,  0,  0,  0,  0,
                                                0,  0,  0,  0,  0,  0,  0,  0,
                                                0,  0,  3,  5,  5,  3,  0,  0,
                                                0,  0,  5, 10, 10,  5,  0,  0,
                                                0,  5,  8, 10, 10,  8,  5,  0,
                                                0,  2,  2,  5,  5,  2,  2,  0,
                                                0,  0,  0,  0,  0,  0,  0,  0},

                                              { 0,  0,  0,  0,  0,  0,  0,  0,
                                                0,  2,  2,  5,  5,  2,  2,  0,
                                                0,  5,  8, 10, 10,  8,  5,  0,
                                                0,  0,  5, 10, 10,  5,  0,  0,
                                                0,  0,  3,  5,  5,  3,  0,  0,
                                                0,  0,  0,  0,  0,  0,  0,  0,
                                                0,  0,  0,  0,  0,  0,  0,  0,
                                                0,  0,  0,  0,  0,  0,  0,  0}};

    // Mobility
    const int knightMobilityBonus[9] =  {-35, -25, -8, -4, 5, 8, 12, 18, 25};
    const int bishopMobilityBonus[14] = {-25, -15, 8, 12, 16, 18, 20, 25, 32, 40, 45, 47, 52, 57};
    const int rookMobilityBonus[15] =   {-32, -15, 1, 4, 5, 8, 12, 14, 15, 21, 27, 30, 34, 36, 42};
    const int queenMobilityBonus[27] =  {-15, -15, -10, -5, 12, 12, 15, 16, 18, 21, 26, 29, 35, 35, 37, 37, 42, 42, 43, 43, 49, 55, 62, 66, 69, 73, 79};

    // King safety
    const int pieceAttackValue[5] = {0, 20, 25, 42, 75};
    const int pieceAttackWeight[8] = {0, 0, 50, 75, 88, 94, 97, 100};

    // Knight and rook weights
    const int knightWeight[9] = {-35, -28, -12, -8, -4, 0, 12, 17, 27};
    const int rookWeight[9] = {32, 26, 15, 8, 4, 0, -16, -23, -25};

    // Passed Pawn weights
    const int passedPawnWeight[7] = {S(0, 0), S(6, 12), S(8, 15), S(9, 16), S(32, 38), S(87, 96), S(122, 125)};

    // Supported and adjacent pawn weights
    const int supportedPawnWeight[7] = {S(0, 0), S(0, 0), S(12, 25), S(15, 38), S(20, 42), S(32, 88), S(94, 115)};
    const int adjacentPawnWeight[7]  = {S(0, 0), S(3, 12), S(7, 21), S(9, 22), S(13, 25), S(18, 32), S(20, 34)};


};
