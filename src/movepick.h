#pragma once
#include "defs.h"
#include "movegen.h"
#include "bitboard.h"


class MovePick {

public:
    MovePick();
    void scoreMoves(MoveList &moveList, Bitboard &b, ThreadSearch *th, int depth, MOVE pvMove);

private:

    void InitMvvLva();
    int mvvlva[6][6];

};
