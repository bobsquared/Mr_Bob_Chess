
#pragma once
#include <vector>
#include "bitboard.h"
#include "defs.h"



#define EXACT 0
#define LOWER_BOUND 1
#define UPPER_BOUND 2


class TranspositionTable{

public:

    ~TranspositionTable();
    TranspositionTable();
    TranspositionTable(int hashSize);

    void setSize(int hashSize);

    void saveTT(ThreadSearch *th, MOVE move, int score, int staticScore, int depth, uint8_t flag, uint64_t key, int ply);
    bool probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, int alpha, int beta, int ply);
    bool probeTTQsearch(uint64_t key, ZobristVal &hashedBoard, bool &ttRet, int alpha, int beta, int ply);
    int getHashFull(uint64_t writes);
    void clearHashTable();
    ZobristVal getHashValue(uint64_t posKey);
    void setTTAge(int age);
    std::string getPv(Bitboard &b);



private:

    int halfMove;
    ZobristVal *hashTable;
    uint64_t numHashes;
    uint64_t ttWrites;


};
