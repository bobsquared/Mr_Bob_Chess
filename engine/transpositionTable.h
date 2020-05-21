
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "zobrist_hashing.h"
#include "bitboard.h"
#include <cassert>
#include "config.h"
#include <algorithm>
#include "defs.h"



class TranspositionTable{

public:

  // This is what is stored in the transposition table
  struct ZobristVal {
    Bitboard::Move move;
    int16_t score;
    int8_t depth;
    uint8_t flag;
    uint64_t posKey;
    uint16_t halfMove;

    ZobristVal() :
      move(Bitboard::Move()), score(0), depth(0), flag(0), posKey(0), halfMove(0) {}

    ZobristVal(Bitboard::Move move, int16_t score, int8_t depth, uint8_t flag, uint64_t posKey, uint16_t halfMove) :
      move(move), score(score), depth(depth), flag(flag), posKey(posKey), halfMove(halfMove) {}
  };

  ~TranspositionTable();
  TranspositionTable();

  void saveTT(Bitboard::Move &move, int score, int depth, uint8_t flag, uint64_t key);
  bool probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, int &alpha, int &beta);
  std::string getPV(Bitboard &bitboard);
  void updateHalfMove();
  void getHashStats();



private:

  int halfMove;
  ZobristVal *hashTable;
  uint64_t numHashes;

  uint64_t ttHits;
  uint64_t ttCalls;

  uint64_t ttOverwrites;
  uint64_t ttWrites;


};
