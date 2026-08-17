
#pragma once
#include <vector>
#include <stack>
#include "board/bitboard.h"
#include "defs.h"
#include "thread_search.h"



#define EXACT 0
#define LOWER_BOUND 1
#define UPPER_BOUND 2

// This is what is stored in the transposition table
struct TTEntry {
    uint32_t posKey;
    MOVE move;
    MOVE move2;
    MOVE move3;
    int16_t score;
    int16_t staticScore;
    uint8_t flagsAndAge;
    int8_t depth;

    TTEntry() : posKey(0), move(NO_MOVE), move2(NO_MOVE), move3(NO_MOVE), score(0), staticScore(0), flagsAndAge(0), depth(0) {}

    TTEntry(uint64_t posKey, MOVE move, int16_t score, int16_t staticScore, uint8_t flagsAndAge, int8_t depth) :
        posKey(posKey), move(move), move2(NO_MOVE), move3(NO_MOVE), score(score), staticScore(staticScore), flagsAndAge(flagsAndAge), depth(depth) {}
};

struct alignas(64) TTBucket {
    TTEntry entries[4];
};

struct TranspositionTable {
    TTBucket *hashTable;
    uint64_t mask;
    uint64_t numHashes;
    uint64_t ttWrites;
    uint8_t age;
};

static_assert(sizeof(TTBucket) == 64, "TTBucket size is not 64 bytes");


namespace TT {

    extern TranspositionTable tt;

    void InitTT(uint64_t hashSize);
    void DestroyTT();
    void setSize(uint64_t hashSize);
    void saveTT(ThreadData& td, MOVE move, int score, int staticScore, int depth, uint8_t flag, uint64_t key, int ply);
    bool probeTT(uint64_t key, TTEntry &hashedBoard, int depth, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply);
    bool probeTTQsearch(uint64_t key, TTEntry &hashedBoard, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply);
    int getHashFull(uint64_t writes);
    void clearHashTable();
    void incrementTTAge();
    void saveTTSecondary(uint64_t key, MOVE move);
    std::string getPv(Board &b);

    inline uint8_t getAgeFromTT(uint8_t flagsAndAge) {
        return flagsAndAge >> 2;
    }
    
    inline uint8_t getFlagsFromTT(uint8_t flagsAndAge) {
        return flagsAndAge & 0b11;
    }

    inline uint8_t setFlagsAndAgeInTT(uint8_t age, uint8_t flag) {
        return (age << 2) | flag;
    }

    inline void prefetchTT(uint64_t key) {
        __builtin_prefetch(&tt.hashTable[key & tt.mask], 0, 3);
    }
}