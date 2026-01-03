#include "detections.h"

namespace BITBOARD {
    /************************************************************************************************
    **  Draws and Repetition section
    **  Used for determining draws.
    *************************************************************************************************/


    // Returns a boolean on whether the position is a draw.
    bool isDraw(BoardState& bs, MoveInfoStack& moveHistory, int ply) {

        // 50 move rule
        if (bs.halfMoves >= 100) {
            return true;
        }

        // Repetition
        bool repetition = false;
        for (int i = moveHistory.count - 2; i >= std::max(0, moveHistory.count - bs.halfMoves) ; i -= 2) {
            if (moveHistory.moves[i].posKey == bs.posKey) {
                if (repetition || ply > 1) {
                    return true;
                }
                repetition = true;
            }
        }


        // Material draw.
        if (bs.pieceCount[0] + bs.pieceCount[1] + bs.pieceCount[6] + bs.pieceCount[7] + bs.pieceCount[8] + bs.pieceCount[9] > 0) {
            return false;
        }

        // Material draw.
        if (bs.pieceCount[2] + bs.pieceCount[3] + bs.pieceCount[4] + bs.pieceCount[5] <= 1) {
            return true;
        }

        return false;
    }



    // Returns true if there are no potential win material (only knight, only bishop)
    bool noPotentialWin(BoardState& bs) {

        if (bs.pieceCount[bs.toMove] + bs.pieceCount[6 + bs.toMove] + bs.pieceCount[8 + bs.toMove] > 0) {
            return false;
        }

        if (bs.pieceCount[2 + bs.toMove] + bs.pieceCount[4 + bs.toMove] == 1) {
            return true;
        }

        return false;
    }

}