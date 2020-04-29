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
#include "transpositionTable.h"
#include <cassert>

extern uint64_t traversedNodes;
const int MATE_VALUE = 32500;
const int INFINITY_VAL = 1000000;

// Debugging variables
extern uint64_t pruning;
extern uint64_t pruningTotal;
extern uint64_t pruningTT;
extern uint64_t pruningTotalTT;
//---------------------

// Atomic boolean for multithreading stop command
extern std::atomic<bool> exit_thread_flag;




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


ReturnInfo searchRoot(bool whiteMove, Bitboard &bitboard, TranspositionTable &tt, int depth, int &seldepth, std::vector<Bitboard::Move> &vMoves, int alpha=-INFINITY_VAL, int beta=INFINITY_VAL, bool isMain=true);
#endif
