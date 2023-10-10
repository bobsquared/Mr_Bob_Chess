#pragma once


#include "defs.h"
#include "bitboard.h"


class ThreadSearch {
public:

    ThreadSearch();
    void clearData();
    void InitKillers();
    void insertKiller(int ply, MOVE move);
    void removeKiller(int ply);
    void ResetHistories();
    bool isKiller(int ply, MOVE move);
    void InitCounterMoves();
    void insertCounterMove(Bitboard &b, MOVE move);
    int getHistory(bool toMove, bool isQuiet, int from, int to);
    int getCounterHistory(Bitboard &b, PrevMoveInfo &prev, int from, int to);
    MOVE getCounterMove(Bitboard &b, PrevMoveInfo &prev);
    void UpdateHistories(Bitboard &b, PrevMoveInfo &prev, MOVE *quietMoves, MOVE *noisyMoves, int quietCount, int noisyCount, int depth, MOVE ttMove, MOVE bestMove);
    
    uint64_t ttWrites;
    uint64_t nodes;

    SearchStack searchStack[MAX_PLY] = {};
    int seldepth;
    MOVE bestMove;
    bool nullMoveTree;

    MOVE killers[MAX_PLY][2] = {};
    int quietHistory[2][64][64] = {};
    int captureHistory[2][64][64] = {};
    int counterHistory[2][6][64][6][64] = {};
    MOVE counterMove[2][64][64] = {};
};