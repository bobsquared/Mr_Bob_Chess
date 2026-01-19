#pragma once


#include "defs.h"
#include "board/bitboard.h"


class ThreadSearch {
public:

    ThreadSearch();
    void clearData();
    void InitKillers();
    void ResetHistories();
    void InitCounterMoves();
    void UpdateHistories(Board &b, PrevMoveInfo &prev, MOVE *quietMoves, MOVE *noisyMoves, int quietCount, int noisyCount, int depth, MOVE ttMove, MOVE bestMove);



    /**
    * Insert a move into killers array for a given ply
    *
    * @param[in, out] th   A pointer to the thread data that called the function.
    * @param[in]      ply  The current ply/height that the search is at.
    * @param[in]      move The move to be inserted into killers array.
    */
    inline void insertKiller(int ply, MOVE move) {
        if (killers[ply][0] == move) {
            return;
        }
        killers[ply][1] = killers[ply][0];
        killers[ply][0] = move;
    }



    /**
    * Removes killer moves for a given ply
    *
    * @param[in, out] th  A pointer to the thread data that called the function.
    * @param[in]      ply The current ply/height that the search is at.
    */
    inline void removeKiller(int ply) {
        killers[ply][1] = NO_MOVE;
        killers[ply][0] = NO_MOVE;
    }



    /**
    * Checks to see if a move is a killer move
    *
    * @param[in] th   A pointer to the thread data that called the function.
    * @param[in] ply  The current ply/height that the search is at.
    * @param[in] move The move to be determined if it is a killer move.
    */
    inline bool isKiller(int ply, MOVE move) {
        return killers[ply][0] == move || killers[ply][1] == move;
    }



    /**
     * @brief Get the History object
     * 
     * @param move 
     * @return int 
     */
    inline int getHistory(bool toMove, bool isQuiet, int from, int to) {
        return isQuiet? quietHistory[toMove][from][to] : captureHistory[toMove][from][to];
    }


    /**
     * @brief 
     * 
     * @param b 
     * @param move 
     * @return int 
     */
    inline int getCounterHistory(Board &b, PrevMoveInfo &prev, int from, int to) {
        if (prev.prevMove == NULL_MOVE) {
            return 0;
        }

        bool toMove = b.state.toMove;

        return counterHistory[toMove][prev.prevPiece][prev.prevMoveTo][b.state.pieceAt[from] / 2][to];
    }

    /**
     * @brief Get the Counter Move object
     * 
     * @param b 
     * @param move 
     * @return MOVE 
     */
    inline MOVE getCounterMove(Board &b, PrevMoveInfo &prev) {
        MOVE prevMove = prev.prevMove;

        if (prevMove == NULL_MOVE || prevMove == NO_MOVE) {
            return NO_MOVE;
        }

        return counterMove[b.state.toMove][prev.prevMoveFrom][prev.prevMoveTo];
    }
    
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