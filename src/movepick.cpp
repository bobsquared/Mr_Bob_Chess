/**
* A move picker.
*
* Used mainly to give moves a score.
*/


#include "movepick.h"


/**
* A constructor for the move picker.
*/
MovePick::MovePick() {
    InitMvvLva();
}



/**
* An initializer for MvvLva.
*
* Initialize the values for the mvvlva array.
*/
void MovePick::InitMvvLva() {

    for (int i = 0; i < 6; i++) {
        for (int j = 5; j >= 0; j--) {
            mvvlva[i][j] = 100 + j * 100 + (6 - i) * 10;
        }
    }

}



/**
* A move scorer.
*
* Score the moves from the move list based on implemented heuristics.
*
* @param[in, out] moveList The list of moves to be scored.
* @param[in]      b        The board representation.
* @param[in]      th       A pointer to the thread data that called the function.
* @param[in]      ply      The current ply/height that the search is at.
* @param[in]      pvMove   The principal variation move found in the transposition table.
*/
void MovePick::scoreMoves(MoveList &moveList, Bitboard &b, ThreadSearch *th, int ply, MOVE pvMove) {

    MOVE move;
    int from;
    int to;
    MOVE prevMove = b.moveHistory.count > 0? b.moveHistory.move[b.moveHistory.count - 1].move : NO_MOVE;
    int prevMoveTo = get_move_to(prevMove);
    int prevPiece = b.pieceAt[prevMoveTo] / 2;
    bool isValidPrevMove = (prevMove != NO_MOVE && prevMove != NULL_MOVE);

    for (int i = 0; i < moveList.count; i++) {
        moveList.get_index_move(i, move);
        int moveTo = get_move_to(move);
        int moveFrom = get_move_from(move);

        if (move == pvMove) {
            moveList.set_score_index(i, 1500000);
        }
        else if (move & CAPTURE_FLAG) {

            from = b.pieceAt[moveFrom] / 2;
            to = b.pieceAt[moveTo] / 2;

            if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
                moveList.set_score_index(i, 1000000 + mvvlva[from][0]);
            }
            else {
                int see = b.seeCapture(move);
                int score = (see > 0? 1000000 : (see == 0? 950000 : (ply <= 6? 0 :  750000)));
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
        else if (th->killers[ply][0] == move) {
            moveList.set_score_index(i, 900000);
        }
        else if (th->killers[ply][1] == move) {
            moveList.set_score_index(i, 800000);
        }
        else if (th->counterMove[b.toMove][get_move_from(prevMove)][prevMoveTo] == move) {
            moveList.set_score_index(i, 700000);
        }
        else {
            int cmh = isValidPrevMove * th->counterHistory[b.toMove][prevPiece][prevMoveTo][b.pieceAt[moveFrom] / 2][moveTo];
            moveList.set_score_index(i, th->history[b.toMove][moveFrom][moveTo] + cmh);
        }
    }

}



/**
* A move scorer for qsearch.
*
* Score the moves from the move list based on implemented heuristics.
*
* @param[in, out] moveList The list of moves to be scored.
* @param[in]      b        The board representation.
* @param[in]      pvMove   The principal variation move found in the transposition table.
*/
void MovePick::scoreMovesQS(MoveList &moveList, Bitboard &b, MOVE pvMove) {

    MOVE move;
    int from;
    int to;

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
    }

}
