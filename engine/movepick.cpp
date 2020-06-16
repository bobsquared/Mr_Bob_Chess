#include "movepick.h"


// MovePick is used to score the moves such that the search will pick good moves first.
// This is move ordering.
MovePick::MovePick() {
    InitMvvLva();
}



// Initialize MVV/LVA
void MovePick::InitMvvLva() {

    for (int i = 0; i < 6; i++) {
        for (int j = 5; j >= 0; j--) {
            mvvlva[i][j] = 100 + j * 100 + (6 - i) * 10;
            if (j >= i) {
                mvvlva[i][j] *= 10;
            }
        }
    }

}



// Set a score to all the moves in the movelist.
// Higher score moves are picked first
void MovePick::scoreMoves(MoveList &moveList, int *pieceAt, int depth, bool col, MOVE pvMove) {

    MOVE move;
    int from;
    int to;

    for (int i = 0; i < moveList.count; i++) {
        moveList.get_index_move(i, move);

        if (move == pvMove) {
            moveList.set_score_index(i, 1500000);
        }
        else if (move & CAPTURE_FLAG) {

            from = pieceAt[get_move_from(move)] / 2;
            to = pieceAt[get_move_to(move)] / 2;

            if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
                moveList.set_score_index(i, 1000000 + mvvlva[from][0]);
            }
            else {
                int score = mvvlva[from][to] >= 1000? 1000000 : 700000;
                moveList.set_score_index(i, score + mvvlva[from][to]);
            }

        }
        else if (move & PROMOTION_FLAG) {
            moveList.set_score_index(i, 1000000);
        }
        else if ((move & MOVE_BITS) == (killers[col][depth][0] & MOVE_BITS)) {
            moveList.set_score_index(i, 900000);
        }
        else if ((move & MOVE_BITS) == (killers[col][depth][1] & MOVE_BITS)) {
            moveList.set_score_index(i, 800000);
        }
        else {
            moveList.set_score_index(i, history[col][get_move_from(move)][get_move_to(move)]);
        }
    }

}
