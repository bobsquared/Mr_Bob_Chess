#pragma once
#include "defs.h"
#include "dumb7flooding.h"



namespace MAGIC_BITBOARDS{
    struct alignas(64) MagicPro {
        uint64_t magic;
        uint64_t mask;
        uint8_t shift;
        uint8_t padding[64 - 8 - 8 - 1];
    };

    extern MagicPro attacksR[64];
    extern MagicPro attacksB[64];

    extern uint64_t rookComb[64 * 4096];
    extern uint64_t bishopComb[64 * 512];

    extern void Generate_Magic_Rooks();
    extern void Generate_Magic_Bishops();
    extern void InitMagicBitboards();
    extern uint64_t xrayAttackRook(uint64_t occ, uint64_t blockers, int index);
    extern uint64_t xrayAttackBishop(uint64_t occ, uint64_t blockers, int index);

    static inline uint64_t bishopAttacksMask(uint64_t occupations, int index){
        occupations &= attacksB[index].mask;
        occupations = ((attacksB[index].magic * occupations) >> attacksB[index].shift);
        return bishopComb[((uint64_t)index << 9) + occupations];
    }

    static inline uint64_t rookAttacksMask(uint64_t occupations, int index) {
        occupations &= attacksR[index].mask;
        occupations = ((attacksR[index].magic * occupations) >> attacksR[index].shift);
        return rookComb[((uint64_t)index << 12) + occupations];
    }

    static inline uint64_t queenAttacksMask(uint64_t occupations, int index) {
        return bishopAttacksMask(occupations, index) | rookAttacksMask(occupations, index);
    }

    

};
