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
    Eval();
    int evaluate(Bitboard &board);
    int evaluate_debug(Bitboard &board);

private:

    void InitializeEval(Bitboard &board);
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
    int evaluatePawns(Bitboard &board, bool col);
    int evaluateKnights(Bitboard &board, bool col);
    int evaluateBishops(Bitboard &board, bool col);
    int evaluateRooks(Bitboard &board, bool col);
    int evaluateQueens(Bitboard &board, bool col);
    int evaluateKing(Bitboard &board, bool col);
    int evaluatePawnShield(Bitboard &board, bool col);
    int evaluateThreats(Bitboard &board, bool col);

    void savePawnHash(uint64_t key, int score);
    int probePawnHash(uint64_t key, bool &hit);

    int getPhase(Bitboard &board);

    int pieceSquare[12][64];
    int pieceSquareEG[12][64];
    uint64_t kingZoneMask[2][64];
    uint64_t passedPawnMask[2][64];
    uint64_t forwardMask[2][64];
    int manhattanArray[64][64];
    int chebyshevArray[64][64];
    uint64_t isolatedPawnMask[64];
    uint64_t outpostMask[2][64];
    uint64_t knightOutpost[2][64];


    // King safety
    const int pieceAttackWeight[8] = {0, 0, 50, 75, 88, 94, 97, 100};

    uint64_t attacksKnight[2];
    uint64_t attacksBishop[2];
    uint64_t attacksRook[2];
    uint64_t attacksQueen[2];

    uint64_t unsafeSquares[2];
    int KSAttackersCount[2];
    int KSAttackersWeight[2];
    int KSAttacks[2];

    uint64_t mobilityUnsafeSquares[2];
    uint64_t minorUnsafe[2];
    uint64_t queenUnsafe[2];
    uint64_t tempUnsafe[2];



    struct PawnHash{
        uint64_t pawnKey;
        int score;

        PawnHash() :
            pawnKey(0), score(0) {};

        PawnHash(uint64_t pawnKey, int score) :
            pawnKey(pawnKey), score(score) {};
    };


    uint64_t numPawnHashes;
    PawnHash *pawnHash;

    bool hit;
    int pawnScore;

};
