
#pragma once
#include <vector>
#include <stack>
#include "board/bitboard.h"
#include "board/move.h"
#include "defs.h"
#include "thread_search.h"



#define EXACT 0
#define LOWER_BOUND 1
#define UPPER_BOUND 2

// This is what is stored in the transposition table
struct ZobristVal {
    uint64_t posKey;
    MOVE move;
    int16_t score;
    int16_t staticScore;
    uint8_t flagsAndAge;
    int8_t depth;

    ZobristVal() : posKey(0), move(NO_MOVE), score(0), staticScore(0), flagsAndAge(0), depth(0) {}

    ZobristVal(MOVE move, int16_t score, int16_t staticScore, int8_t depth, uint8_t flagsAndAge, uint64_t posKey) :
        posKey(posKey), move(move), score(score), staticScore(staticScore), flagsAndAge(flagsAndAge), depth(depth) {}
};


struct alignas(64) Bucket {
    ZobristVal entries[4] = {};

    Bucket() {
        for (int i = 0; i < 4; i++) {
            entries[i] = ZobristVal();
        }
    }
};

static_assert(sizeof(Bucket) == 64, "Bucket size is not 64 bytes");

class TranspositionTable{

public:

    ~TranspositionTable();
    TranspositionTable();
    TranspositionTable(int hashSize);

    void setSize(uint64_t hashSize);

    void saveTT(ThreadSearch *th, MOVE move, int score, int staticScore, int depth, uint8_t flag, uint64_t key, int ply);
    bool probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply);
    bool probeTTQsearch(uint64_t key, ZobristVal &hashedBoard, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply);
    int getHashFull(uint64_t writes);
    void clearHashTable();
    void incrementTTAge();
    std::string getPv(Board &b);



private:

    inline uint8_t getAgeFromTT(uint8_t flagsAndAge) {
        return flagsAndAge >> 2;
    }
    
    inline uint8_t getFlagsFromTT(uint8_t flagsAndAge) {
        return flagsAndAge & 0b11;
    }

    inline uint8_t setFlagsAndAgeInTT(uint8_t age, uint8_t flag) {
        return (age << 2) | flag;
    }
    
    ZobristVal *hashTable;
    uint64_t numHashes;
    uint64_t ttWrites;
    uint8_t age;

};
