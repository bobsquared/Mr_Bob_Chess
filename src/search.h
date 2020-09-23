#pragma once
#include <atomic>
#include <chrono>
#include "defs.h"
#include "eval.h"
#include "bitboard.h"
#include "movepick.h"
#include "movegen.h"
#include "timeman.h"
#include <cmath>



#define ASPIRATION_DELTA 11


struct BestMoveInfo {
    uint16_t move;
    int eval;

    BestMoveInfo(uint16_t move, int eval) : move(move), eval(eval) {}
};


struct SearchStack {
    int eval;
};


extern std::atomic<bool> exit_thread_flag;
extern uint64_t nodes;
extern int totalTime;
extern bool printInfo;

int pvSearch(Bitboard &b, int depth, int alpha, int beta);
extern BestMoveInfo pvSearchRoot(Bitboard &b, int depth, int alpha, int beta);
extern void search(Bitboard &b, int depth, int wtime, int btime, int winc, int binc, int movesToGo, bool analysis);
extern void InitLateMoveArray();
extern int qsearch(Bitboard &b, int depth, int alpha, int beta, int height);
