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


const int pieceValues[6] = {100, 315, 320, 525, 1025, 2000};

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
    uint64_t adjacentMask(uint64_t pawns);
    int evaluate_piece_square_values(uint64_t *pieces, bool eg, bool col);
    int evaluateTrappedRook(uint64_t *pieces, bool col);
    int evaluateMobility(uint64_t *pieces, Magics *magics, uint64_t *knightMoves, uint64_t occupied, bool col);
    int evaluateKingSafety(uint64_t *pieces, Magics *magics, uint64_t *knightMoves, uint64_t occupied, bool col);
    int evaluateImbalance(int *pieceCount, bool col);
    int evaluatePawns(uint64_t *pieces, bool col);
    int evaluatePassedPawns(uint64_t *pieces, bool col);
    int pieceSquare[12][64];
    int pieceSquareEG[12][64];
    uint64_t kingZoneMask[2][64];
    uint64_t passedPawnMask[2][64];
    uint64_t forwardMask[2][64];
    int manhattanArray[64][64];
    int chebyshevArray[64][64];

    // Mobility
    const int knightMobilityBonus[9] =  {-35, -25, -8, -4, 5, 8, 12, 18, 25};
    const int bishopMobilityBonus[14] = {-25, -15, 8, 12, 16, 18, 20, 25, 32, 40, 45, 47, 52, 57};
    const int rookMobilityBonus[15] =   {-32, -15, 1, 4, 5, 8, 12, 14, 15, 21, 27, 30, 34, 36, 42};
    const int queenMobilityBonus[27] =  {-15, -15, -10, -5, 12, 12, 15, 16, 18, 21, 26, 29, 35, 35, 37, 37, 42, 42, 43, 43, 49, 49, 55, 62, 66, 67, 69};

    // King safety
    const int pieceAttackValue[5] = {0, 20, 25, 42, 75};
    const int pieceAttackWeight[8] = {0, 0, 50, 75, 88, 94, 97, 100};

    // Knight and rook weights
    const int knightWeight[9] = {-35, -28, -12, -8, -4, 0, 12, 17, 27};
    const int rookWeight[9] = {32, 26, 15, 8, 4, 0, -16, -23, -25};


};
