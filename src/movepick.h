/**
* A move picker.
*
* Used mainly to give moves a score.
*/


#pragma once
#include "defs.h"
#include "movegen.h"
#include "bitboard.h"
#include "thread_search.h"

class MovePick {

public:

    /**
    * A constructor for the move picker.
    */
    MovePick();

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
    void scoreMoves(MoveList &moveList, Bitboard &b, PrevMoveInfo &prev, ThreadSearch *th, int ply, MOVE pvMove);

    /**
    * A move scorer for qsearch.
    *
    * Score the moves from the move list based on implemented heuristics.
    *
    * @param[in, out] moveList The list of moves to be scored.
    * @param[in]      b        The board representation.
    * @param[in]      pvMove   The principal variation move found in the transposition table.
    */
    void scoreMovesQS(MoveList &moveList, Bitboard &b, MOVE pvMove);

private:

    int mvvlva[6][6]; /**< The array containing values for most valuable victim, least valuable attacker.*/

    /**
    * An initializer for MvvLva.
    *
    * Initialize the values for the mvvlva array.
    */
    void InitMvvLva();

};
