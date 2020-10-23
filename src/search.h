#pragma once
#include <atomic>
#include <chrono>
#include <deque>
#include "defs.h"
#include "eval.h"
#include "bitboard.h"
#include "movepick.h"
#include "movegen.h"
#include "timeman.h"
#include <cmath>
#include <thread>



#define ASPIRATION_DELTA 8


struct BestMoveInfo {
    MOVE move;
    int eval;

    BestMoveInfo(uint16_t move, int eval) : move(move), eval(eval) {}
};


extern std::atomic<bool> exit_thread_flag;

extern int totalTime;
extern bool printInfo;
extern ThreadSearch thread[256];
extern int nThreads;

extern void beginSearch(Bitboard &b, int depth, int wtime, int btime, int winc, int binc, int movesToGo, bool analysis);
extern void InitLateMoveArray();
extern uint64_t getTotalNodesSearched();
extern int qsearch(Bitboard &b, int depth, int alpha, int beta, int height);
