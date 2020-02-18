#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <atomic>
#include "bitboard.h"
#include "zobrist_hashing.h"
#include <cassert>

extern uint64_t traversedNodes;
const int MATE_VALUE = 100000;

// Debugging variables
extern uint64_t pruning;
extern uint64_t pruningTotal;
extern uint64_t pruningTT;
extern uint64_t pruningTotalTT;
//---------------------

// Atomic boolean for multithreading stop command
extern std::atomic<bool> exit_thread_flag;


// Number to algebra
extern const std::string TO_ALG[64];

// Algebra to number
extern std::unordered_map<std::string, uint8_t> TO_NUM;

struct ReturnInfo {
  std::string bestMove;
  Bitboard::Move move;
  int score;
  int mateIn;
  ReturnInfo() :
    bestMove(""), move(Bitboard::Move()), score(0), mateIn(0) {}

  ReturnInfo(std::string bestMove, Bitboard::Move move, int score, int mateIn) :
    bestMove(bestMove), move(move), score(score), mateIn(mateIn) {}
};


int searchR(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta);
ReturnInfo searchRoot(bool whiteMove, Bitboard &bitboard, int depth, std::vector<Bitboard::Move> &vMoves, int alpha=-100000000, int beta=100000000, bool isMain=true);
#endif
