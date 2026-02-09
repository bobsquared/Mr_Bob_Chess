/**
* A move picker.
*
* Used mainly to give moves a score.
*/


#pragma once
#include "../defs.h"
#include "movegen.h"
#include "bitboard.h"
#include "../thread_search.h"
#include "movelist.h"
#include "see.h"


enum MovePickStage {
    PV_MOVE = 0,
    PV_MOVE2 = 1,
    PV_MOVE3 = 2,
    GEN_CAPTURES = 3,
    GOOD_CAPTURES = 4,
    KILLER1 = 5,
    KILLER2 = 6,
    COUNTER_MOVE = 7,
    GEN_QUIETS = 8,
    QUIETS = 9,
    BAD_CAPTURES = 10,
    DONE = 11
};


struct MovePickData {
    MoveList moveListCaptures;
    MoveList moveListQuiets;
    enum MovePickStage stage;
    MOVE pvMove;
    MOVE pvMove2;
    MOVE pvMove3;
    MOVE killMove1;
    MOVE killMove2;
    MOVE counterMove;

    MovePickData() : stage(PV_MOVE), pvMove(NO_MOVE), pvMove2(NO_MOVE), pvMove3(NO_MOVE), killMove1(NO_MOVE), killMove2(NO_MOVE), counterMove(NO_MOVE) {}
    MovePickData(MOVE pvMove, MOVE pvMove2, MOVE pvMove3, MOVE killMove1, MOVE killMove2, MOVE counterMove) : 
        stage(PV_MOVE), pvMove(pvMove), pvMove2(pvMove2), pvMove3(pvMove3), killMove1(killMove1), killMove2(killMove2), counterMove(counterMove) {}
    
};


struct MovePickDataQS {
    MoveList moveList;
    enum MovePickStage stage;
    MOVE pvMove;

    MovePickDataQS() : stage(PV_MOVE), pvMove(NO_MOVE) {}
    MovePickDataQS(MOVE pvMove) : stage(PV_MOVE), pvMove(pvMove) {}
};




namespace MOVEPICK {

    void scoreMoves(MoveList &moveList, Board &b, PrevMoveInfo &prev, ThreadData &td);

    bool pick_move(MOVE &move, Board &b, PrevMoveInfo &prev, ThreadData &td, MovePickData &moves);

    bool pick_move_qs(MOVE &move, Board &b, MovePickDataQS &mpd, bool inCheck);

    inline bool skip_move(MOVE m, const MovePickData& mpd) {
        return (m == mpd.pvMove) |
            (m == mpd.pvMove2) |
            (m == mpd.pvMove3) |
            (m == mpd.killMove1) |
            (m == mpd.killMove2) |
            (m == mpd.counterMove);
    }

    inline bool skip_move_qs(MOVE m, const MovePickDataQS& mpd) {
        return (m == mpd.pvMove);
    }

    inline constexpr int InitMvvLva(int from, int to) {
        return 100 + to * 100 - from * 10;
    }

    inline constexpr int mvvlva[6][6] = {
        {InitMvvLva(0, 0), InitMvvLva(0, 1), InitMvvLva(0, 2), InitMvvLva(0, 3), InitMvvLva(0, 4), InitMvvLva(0, 5)},
        {InitMvvLva(1, 0), InitMvvLva(1, 1), InitMvvLva(1, 2), InitMvvLva(1, 3), InitMvvLva(1, 4), InitMvvLva(1, 5)},
        {InitMvvLva(2, 0), InitMvvLva(2, 1), InitMvvLva(2, 2), InitMvvLva(2, 3), InitMvvLva(2, 4), InitMvvLva(2, 5)},
        {InitMvvLva(3, 0), InitMvvLva(3, 1), InitMvvLva(3, 2), InitMvvLva(3, 3), InitMvvLva(3, 4), InitMvvLva(3, 5)},
        {InitMvvLva(4, 0), InitMvvLva(4, 1), InitMvvLva(4, 2), InitMvvLva(4, 3), InitMvvLva(4, 4), InitMvvLva(4, 5)},
        {InitMvvLva(5, 0), InitMvvLva(5, 1), InitMvvLva(5, 2), InitMvvLva(5, 3), InitMvvLva(5, 4), InitMvvLva(5, 5)}
    };


}
