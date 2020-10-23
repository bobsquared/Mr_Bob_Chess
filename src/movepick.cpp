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
        }
    }

}



// Set a score to all the moves in the movelist.
// Higher score moves are picked first
void MovePick::scoreMoves(MoveList &moveList, Bitboard &b, ThreadSearch *th, int depth, MOVE pvMove) {

    MOVE move;
    int from;
    int to;
    MOVE prevMove = b.moveHistory.move[b.moveHistory.count - 1].move;

    for (int i = 0; i < moveList.count; i++) {
        moveList.get_index_move(i, move);

        if (move == pvMove) {
            moveList.set_score_index(i, 1500000);
        }
        else if (move & CAPTURE_FLAG) {

            from = b.pieceAt[get_move_from(move)] / 2;
            to = b.pieceAt[get_move_to(move)] / 2;

            if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
                moveList.set_score_index(i, 1000000 + mvvlva[from][0]);
            }
            else {
                int score = to > from? 1000000 : (to == from? 975000 : 950000);
                moveList.set_score_index(i, score + mvvlva[from][to]);
            }

        }
        else if (move & PROMOTION_FLAG) {
            if ((move & QUEEN_PROMOTION_FLAG) == QUEEN_PROMOTION_FLAG) {
                moveList.set_score_index(i, 1000000);
            }
            else if ((move & QUEEN_PROMOTION_FLAG) == KNIGHT_PROMOTION_FLAG) {
                moveList.set_score_index(i, 650000);
            }
            else {
                moveList.set_score_index(i, 0);
            }
        }
        else if (th->killers[depth][0] == move) {
            moveList.set_score_index(i, 900000);
        }
        else if (th->killers[depth][1] == move) {
            moveList.set_score_index(i, 800000);
        }
        else if (th->counterMove[b.toMove][get_move_from(prevMove)][get_move_to(prevMove)] == move) {
            moveList.set_score_index(i, 700000);
        }
        else {
            moveList.set_score_index(i, th->history[b.toMove][get_move_from(move)][get_move_to(move)]);
        }
    }

}
