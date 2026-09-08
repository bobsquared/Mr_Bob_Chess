#pragma once


#include "defs.h"
#include "board/bitboard.h"



// Information for search
struct SearchStackData {
    int eval;
    int extLevel;
    int hashLevel;
    MOVE singMove;
    
    SearchStackData() : eval(0), extLevel(0), hashLevel(0), singMove(NO_MOVE) {};
};


struct HistoryData {
    int16_t quietHistory[2][64][64] = {};
    int16_t captureHistory[2][64][64] = {};
    int16_t counterHistory[2][6][64][6][64] = {};
    MOVE killers[MAX_PLY][2] = {};
    MOVE counterMove[2][64][64] = {};
};


struct alignas(64) ThreadData {
    HistoryData historyData;
    SearchStackData searchStack[MAX_PLY] = {};

    uint64_t ttWrites;
    uint64_t tbHits;
    uint64_t nodes;
    int seldepth;
    int id;
    MOVE bestMove;
    bool nullMoveTree;
};



namespace THREAD {

    void ClearData();
    void InitKillers();
    void ResetHistories();
    void InitCounterMoves();
    int getNThreads();
    void setNThreads(int n);
    void UpdateHistories(const Board &b, const PrevMoveInfo &prev, HistoryData &hd, const MOVE *quietMoves, const MOVE *noisyMoves, int quietCount, int noisyCount, int depth, MOVE ttMove, MOVE bestMove);



    extern std::vector<ThreadData> threadData;



    inline void insertKiller(HistoryData& hd, int ply, MOVE move) {
        if (hd.killers[ply][0] == move) {
            return;
        }
        hd.killers[ply][1] = hd.killers[ply][0];
        hd.killers[ply][0] = move;
    }



    inline void removeKiller(HistoryData& hd, int ply) {
        hd.killers[ply][1] = NO_MOVE;
        hd.killers[ply][0] = NO_MOVE;
    }



    inline bool isKiller(HistoryData& hd, int ply, MOVE move) {
        return hd.killers[ply][0] == move || hd.killers[ply][1] == move;
    }



    inline int getHistory(const HistoryData& hd, bool toMove, bool isQuiet, int from, int to) {
        return isQuiet? hd.quietHistory[toMove][from][to] : hd.captureHistory[toMove][from][to];
    }



    inline int getCounterHistory(Board &b, const PrevMoveInfo &prev, const HistoryData& hd, int from, int to) {
        if (prev.prevMove == NULL_MOVE) {
            return 0;
        }

        bool toMove = b.state.toMove;

        return hd.counterHistory[toMove][prev.prevPiece][prev.prevMoveTo][b.state.pieceAt[from] / 2][to];
    }



    inline MOVE getCounterMove(Board &b, const PrevMoveInfo &prev, const HistoryData& hd) {
        MOVE prevMove = prev.prevMove;

        if (prevMove == NULL_MOVE || prevMove == NO_MOVE) {
            return NO_MOVE;
        }

        return hd.counterMove[b.state.toMove][prev.prevMoveFrom][prev.prevMoveTo];
    }
    
}