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
 * @param depth 
 * @param ttMove 
 * @param bestMove 
 */
void ThreadSearch::UpdateHistories(Board &b, PrevMoveInfo &prev, MOVE *quietMoves, MOVE *noisyMoves, int quietCount, int noisyCount, int depth, MOVE ttMove, MOVE bestMove) {
    bool toMove = b.state.toMove;
    int bestMoveFrom = get_move_from(bestMove);
    int bestMoveTo = get_move_to(bestMove);
    int piece = b.state.pieceAt[bestMoveFrom] / 2;
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
            counterMove[b.state.toMove][prev.prevMoveFrom][prevMoveTo] = bestMove;
        }

        for (int i = 0; i < quietCount; i++) {
            MOVE move = quietMoves[i];
            int from = get_move_from(move);
            int to = get_move_to(move);
            piece = b.state.pieceAt[from] / 2;

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

        int hist = captureHistory[b.state.toMove][noisyFrom][noisyTo] * std::min(depth, 20) / 23;
        captureHistory[b.state.toMove][noisyFrom][noisyTo] += 32 * (-depth * depth) - hist;
    }

}