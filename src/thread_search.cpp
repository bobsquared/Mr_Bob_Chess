#include "thread_search.h"




/**
 * @brief Construct a new Thread Search:: Thread Search object
 * 
 */
ThreadSearch::ThreadSearch() : ttWrites(0), nodes(0), seldepth(0), bestMove(NO_MOVE), nullMoveTree(true) {
    ResetHistories();
};



/**
 * @brief 
 * 
 */
void ThreadSearch::clearData() {
    ttWrites = 0;
    nodes = 0;
    seldepth = 0;
    bestMove = NO_MOVE;
    nullMoveTree = true;
}



/**
 * @brief 
 * 
 */
void ThreadSearch::InitKillers() {
    for (int i = 0; i < MAX_PLY; i++) {
        for (int j = 0; j < 2; j++) {
            killers[i][j] = 0;
        }
    }
}



/**
 * @brief 
 * 
 */
void ThreadSearch::InitCounterMoves() {

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                counterMove[i][j][k] = 0;
            }
        }
    }

}



/**
 * @brief 
 * 
 */
void ThreadSearch::ResetHistories() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                quietHistory[i][j][k] = 0;
                captureHistory[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 64; k++) {
                for (int l = 0; l < 6; l++) {
                    for (int m = 0; m < 64; m++) {
                        counterHistory[i][j][k][l][m] = 0;
                    }
                }
            }
        }
    }
}



/**
 * @brief 
 * 
 * @param b 
 * @param move 
 */
void ThreadSearch::insertCounterMove(Bitboard &b, MOVE move) {
    MOVE prevMove = b.moveHistory.move[b.moveHistory.count - 1].move;

    if (prevMove != NULL_MOVE) {
        counterMove[b.toMove][get_move_from(prevMove)][get_move_to(prevMove)] = move;
    }
}



/**
* Insert a move into killers array for a given ply
*
* @param[in, out] th   A pointer to the thread data that called the function.
* @param[in]      ply  The current ply/height that the search is at.
* @param[in]      move The move to be inserted into killers array.
*/
void ThreadSearch::insertKiller(int ply, MOVE move) {
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
void ThreadSearch::removeKiller(int ply) {
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
bool ThreadSearch::isKiller(int ply, MOVE move) {
    return killers[ply][0] == move || killers[ply][1] == move;
}



/**
 * @brief Get the History object
 * 
 * @param move 
 * @return int 
 */
int ThreadSearch::getHistory(bool toMove, bool isQuiet, int from, int to) {
    return isQuiet? quietHistory[toMove][from][to] : captureHistory[toMove][from][to];
}


/**
 * @brief 
 * 
 * @param b 
 * @param move 
 * @return int 
 */
int ThreadSearch::getCounterHistory(Bitboard &b, PrevMoveInfo &prev, int from, int to) {
    if (prev.prevMove == NULL_MOVE) {
        return 0;
    }

    bool toMove = b.getSideToMove();

    return counterHistory[toMove][prev.prevPiece][prev.prevMoveTo][b.pieceAt[from] / 2][to];
}

/**
 * @brief Get the Counter Move object
 * 
 * @param b 
 * @param move 
 * @return MOVE 
 */
MOVE ThreadSearch::getCounterMove(Bitboard &b, PrevMoveInfo &prev) {
    MOVE prevMove = prev.prevMove;

    if (prevMove == NULL_MOVE || prevMove == NO_MOVE) {
        return NO_MOVE;
    }

    return counterMove[b.getSideToMove()][prev.prevMoveFrom][prev.prevMoveTo];
}



/**
 * @brief 
 * 
 * @param b 
 * @param depth 
 * @param ttMove 
 * @param bestMove 
 */
void ThreadSearch::UpdateHistories(Bitboard &b, PrevMoveInfo &prev, MOVE *quietMoves, MOVE *noisyMoves, int quietCount, int noisyCount, int depth, MOVE ttMove, MOVE bestMove) {
    bool toMove = b.getSideToMove();
    int bestMoveFrom = get_move_from(bestMove);
    int bestMoveTo = get_move_to(bestMove);
    int piece = b.pieceAt[bestMoveFrom] / 2;
    int histScalar = 32;

    MOVE prevMove = prev.prevMove;
    int prevMoveTo = prev.prevMoveTo;
    int prevPiece = prev.prevPiece;

    if (bestMove == ttMove) {
        histScalar += 8;
    }

    if (isQuietMove(bestMove)) {
        int hist = quietHistory[toMove][bestMoveFrom][bestMoveTo] * std::min(depth, 20) / 23;
        quietHistory[toMove][bestMoveFrom][bestMoveTo] += histScalar * (depth * depth) - hist;

        if (prevMove != NULL_MOVE) {
            hist = counterHistory[toMove][prevPiece][prevMoveTo][piece][bestMoveTo] * std::min(depth, 20) / 23;
            counterHistory[toMove][prevPiece][prevMoveTo][piece][bestMoveTo] += histScalar * (depth * depth) - hist;
            counterMove[b.toMove][prev.prevMoveFrom][prevMoveTo] = bestMove;
        }

        for (int i = 0; i < quietCount; i++) {
            MOVE move = quietMoves[i];
            int from = get_move_from(move);
            int to = get_move_to(move);
            piece = b.pieceAt[from] / 2;

            int hist = quietHistory[toMove][from][to] * std::min(depth, 20) / 23;
            quietHistory[toMove][from][to] += histScalar * (-depth * depth) - hist;

            if (prevMove != NULL_MOVE) {
                hist = counterHistory[toMove][prevPiece][prevMoveTo][piece][to] * std::min(depth, 20) / 23;
                counterHistory[toMove][prevPiece][prevMoveTo][piece][to] += histScalar * (-depth * depth) - hist;
            }
        }
    }
    else {
        int hist = captureHistory[toMove][bestMoveFrom][bestMoveTo] * std::min(depth, 20) / 23;
        captureHistory[toMove][bestMoveFrom][bestMoveTo] += 32 * (depth * depth) - hist;
    }

    for (int i = 0; i < noisyCount; i++) {
        int noisyFrom = get_move_from(noisyMoves[i]);
        int noisyTo = get_move_to(noisyMoves[i]);

        int hist = captureHistory[b.getSideToMove()][noisyFrom][noisyTo] * std::min(depth, 20) / 23;
        captureHistory[b.getSideToMove()][noisyFrom][noisyTo] += 32 * (-depth * depth) - hist;
    }

}