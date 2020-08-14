
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "zobrist_hashing.h"
#include "defs.h"



class TranspositionTable{

public:

    ~TranspositionTable();
    TranspositionTable();
    TranspositionTable(int hashSize);

    void saveTT(MOVE move, int score, int depth, uint8_t flag, uint64_t key, int ply);
    bool probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, int alpha, int beta, int ply);
    int getHashFull();
    void clearHashStats();
    void clearHashTable();
    ZobristVal getHashValue(uint64_t posKey);



private:

    int halfMove;
    ZobristVal *hashTable;
    uint64_t numHashes;

    uint64_t ttHits;
    uint64_t ttCalls;

    uint64_t ttOverwrites;
    uint64_t ttWrites;


};
