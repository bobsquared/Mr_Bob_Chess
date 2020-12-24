#pragma once
#include "defs.h"
#include "magic_bitboards.h"
#include "bitboard.h"



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

    #ifdef TUNER
    struct EvalTrace{
        int pawnCoeff[2] = {0};
        int knightCoeff[2] = {0};
        int bishopCoeff[2] = {0};
        int rookCoeff[2] = {0};
        int queenCoeff[2] = {0};

        int doubledPawnsCoeff[2] = {0};
        int isolatedPawnsCoeff[2] = {0};

        int passedPawnCoeff[7][2] = {0};
        int supportedPawnsCoeff[7][2] = {0};
        int adjacentPawnsCoeff[7][2] = {0};

        int knightWeightCoeff[9][2] = {0};
        int rookWeightCoeff[9][2] = {0};
        int bishopWeightCoeff[9][2] = {0};

        int knightPairCoeff[2] = {0};
        int rookPairCoeff[2] = {0};
        int noPawnsCoeff[2] = {0};

        int rookOnOpenCoeff[2] = {0};
        int rookOnSemiOpenCoeff[2] = {0};
        int rookOnQueenCoeff[2] = {0};

        int knightMobilityCoeff[9][2] = {0};
        int bishopMobilityCoeff[14][2] = {0};
        int rookMobilityCoeff[15][2] = {0};
        int queenMobilityCoeff[28][2] = {0};

        int pawnPstCoeff[64][2] = {0};
        int knightPstCoeff[64][2] = {0};
        int bishopPstCoeff[64][2] = {0};
        int rookPstCoeff[64][2] = {0};
        int queenPstCoeff[64][2] = {0};
        int kingPstCoeff[64][2] = {0};

        int knightThreatCoeff[5][2] = {0};
        int bishopThreatCoeff[5][2] = {0};
        int rookThreatCoeff[5][2] = {0};
    };

    EvalTrace emptyTrace;
    EvalTrace evalTrace;
    #endif

    Eval();
    ~Eval();
    int evaluate(Bitboard &board, ThreadSearch *th);

    #ifdef TUNER
    void clearTrace();
    #endif

private:

    struct PawnHash{
        uint64_t pawnKey;
        int score;

        PawnHash() :
            pawnKey(0), score(0) {};

        PawnHash(uint64_t pawnKey, int score) :
            pawnKey(pawnKey), score(score) {};
    };

    PawnHash *pawnHash;
    uint64_t numPawnHashes;
    uint64_t kingZoneMask[2][64];
    uint64_t passedPawnMask[2][64];
    uint64_t forwardMask[2][64];
    uint64_t isolatedPawnMask[64];
    uint64_t outpostMask[2][64];
    uint64_t knightOutpost[2][64];
    int pieceSquare[12][64];
    int pieceSquareEG[12][64];
    int manhattanArray[64][64];
    int chebyshevArray[64][64];

    void InitializeEval(Bitboard &board, ThreadSearch *th);
    void InitPieceBoards();
    void InitKingZoneMask();
    void InitPassedPawnsMask();
    void InitForwardBackwardMask();
    void InitDistanceArray();
    void InitOutpostSquares();
    void InitIsolatedPawnsMask();
    void InitOutpostMask();
    uint64_t adjacentMask(uint64_t pawns);
    int evaluateImbalance(Bitboard &board, bool col);
    int evaluatePawns(Bitboard &board, ThreadSearch *th, bool col, bool hit, int &pawnScore);
    int evaluateKnights(Bitboard &board, ThreadSearch *th, bool col);
    int evaluateBishops(Bitboard &board, ThreadSearch *th, bool col);
    int evaluateRooks(Bitboard &board, ThreadSearch *th, bool col);
    int evaluateQueens(Bitboard &board, ThreadSearch *th, bool col);
    int evaluateKing(Bitboard &board, ThreadSearch *th, bool col);
    int evaluatePawnShield(Bitboard &board, bool col);
    int evaluateThreats(Bitboard &board, ThreadSearch *th, bool col);

    void savePawnHash(uint64_t key, int score);
    int probePawnHash(uint64_t key, bool &hit);
    int getPhase(Bitboard &board);

};
