
#pragma once
#include <iostream>
#include "defs.h"



class Magics{

public:

    ~Magics();
    Magics();
    void Generate_Magic_Rooks();
    void Generate_Magic_Bishops();
    uint64_t rookAttacksMask(uint64_t occupations, int index);
    uint64_t bishopAttacksMask(uint64_t occupations, int index);
    uint64_t queenAttacksMask(uint64_t occupations, int index);

    uint64_t xrayAttackRook(uint64_t occ, uint64_t blockers, int index);
    uint64_t xrayAttackBishop(uint64_t occ, uint64_t blockers, int index);

private:

    struct MagicPro {
        uint64_t bitboard;
        uint8_t shift;
        uint64_t magic;
        uint64_t mask;

        MagicPro() :
            bitboard(0), shift(0), magic(0), mask(0) {}

        MagicPro(uint64_t bitboard, uint8_t shift, uint64_t magic, uint64_t mask) :
            bitboard(bitboard), shift(shift), magic(magic), mask(mask) {}
    };

    uint64_t *rookMoves;
    uint64_t *bishopMoves;

    uint64_t bitCombinations(uint64_t index, uint64_t bitboard);
    bool InitBlocksRook(uint64_t bitboard, uint64_t index, uint64_t magic);
    bool InitBlocksBishop(uint64_t bitboard, uint8_t index, uint64_t magic);

    void optimalMagicRook(uint64_t *magicR);
    void optimalMagicBishop(uint64_t *magicB);

    void InitBishopMoves(uint64_t *bishopMoves);
    void InitRookMoves(uint64_t *rookMoves);

    MagicPro attacksR[64];
    MagicPro attacksB[64];

    uint64_t *rookComb;
    uint64_t *bishopComb;


};
