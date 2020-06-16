
#pragma once
#include <unordered_map>
#include <vector>
#include <iostream>
#include "defs.h"



class Magics{

public:

    ~Magics();
    Magics(uint64_t *rookMoves, uint64_t *bishopMoves);
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
        uint32_t shift;
        uint64_t magic;
        uint64_t mask;
    };

    uint64_t *rookMoves;
    uint64_t *bishopMoves;

    uint64_t bitCombinations(uint64_t index, uint64_t bitboard);
    bool InitBlocksRook(uint64_t bitboard, uint64_t index, uint64_t magic);
    bool InitBlocksBishop(uint64_t bitboard, uint8_t index, uint64_t magic);

    void optimalMagicRook();
    void optimalMagicBishop();

    std::unordered_map<uint8_t, uint64_t> magicR = {};
    std::unordered_map<uint8_t, uint64_t> magicB = {};

    MagicPro attacksR[64];
    MagicPro attacksB[64];

    uint64_t *rookComb;
    uint64_t *bishopComb;


};
