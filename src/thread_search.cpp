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