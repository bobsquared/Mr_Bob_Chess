#pragma once
#include <atomic>
#include <chrono>
#include "defs.h"
#include "bitboard.h"
#include <cmath>



#define ASPIRATION_DELTA 19


struct BestMoveInfo {
    uint16_t move;
    int eval;

    BestMoveInfo(uint16_t move, int eval) : move(move), eval(eval) {}
};


extern std::atomic<bool> exit_thread_flag;

int pvSearch(Bitboard &b, int depth, int alpha, int beta);
extern BestMoveInfo pvSearchRoot(Bitboard &b, int depth, int alpha, int beta);
extern void search(Bitboard &b, int depth);
extern void InitLateMoveArray();
